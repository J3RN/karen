/*
 * Karen v1.4.2
 *
 * A script to turn on a coffee maker every morning by flipping a relay. Additionally, it displays the current 
 * time and date on an LCD screen and a piezo speaker beeps when coffee is being made.
 *
 * Author: Jonathan Arnett
 * Modified: 05/31/2013
 *
 * Pins:
 *  Control	6
 *  Down	7
 *  UP		8
 *  Brew	9
 *  Piezo	10
 *  Relay	13
 *  LCD RS	12
 *  LCD EN	11
 *  LCD D4	5
 *  LCD D5	4
 *  LCD D6	3
 *  LCD D7	2
 *
 * For LCD V0, 2,200 ohms works for me (r-r-r).
 */

#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "pitches.h"

// Define version number
#define VERSION "Karen v1.4.2"

// Set pins
#define CONTROL_BUTTON 6
#define DOWN_BUTTON 7
#define UP_BUTTON 8
#define COFFEE_BUTTON 9
#define PIEZO 10
#define RELAY 13

// Delays for button pushing
#define DEBOUNCE 250

// Indexes for accessing specific time vars
#define MONTH 0
#define MONTH_DAY 1
#define WEEKDAY 2
#define HOUR 3
#define MINUTE 4

// Constant for number of time vals
#define NUM_TIME_VALS 5

// String used to clear a line of the LCD
const String clearString = "                ";

// Initialize array of month names
const String months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
						"Sep", "Oct", "Nov", "Dec"};

// Initialize array for days in each month
const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Initialize days to an array of days of the week
const String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const String startTimes[] = {"08:00", "08:00", "08:00", "08:00", "08:00", 
							"08:00", "08:00"};

// Have the coffee pot turn off after a given set of time
const bool autostop = true;
// Time (in milliseconds) for the coffee pot to turn off
const unsigned uint32_t autoStopLength = 360000;    // 6 minutes
// Time to stop brewing if autostop is enabled
uint32_t autoStopTime;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// A timer that allows the clock to function
SimpleTimer timer;

// Vars to make looping through time vars easier
const String timeValNames[] = 
	{"Month", "Month day", "Weekday", "Hour", "Minute"};
// Month day will have to be checked separately because months are weird
const int maxi[] = {12, 0, 7, 24, 60};
int timeVals[] = {0, 1, 0, 0, 0};

// Initialize time strings
String timeString = "";
String brewString = "";
String lastBrewString = "Never";

// Initialize brewing boolean
boolean brewing = false;

void setup() {
	// Start LCD
	lcd.begin(16, 2);

	// Set the pin modes for buttons and relay
	pinMode(CONTROL_BUTTON, INPUT);
	pinMode(UP_BUTTON, INPUT);
	pinMode(DOWN_BUTTON, INPUT);
	pinMode(COFFEE_BUTTON, INPUT);
	pinMode(PIEZO, OUTPUT);
	pinMode(RELAY, OUTPUT);
	
	// Relay is set to LOW by default, so stop it
	stopBrew();

	// Show a start-up splash for two seconds
	lcdWriteBottom("");		// Get rid of brew message
	lcdWriteTop(VERSION);
	delay(2000);

	// Have the user set the time
  	setTime();

	// Update time every minute
	timer.setInterval(60000, updateTime);  // 60,000 milliseconds per minute
}


void loop() {
  // Needed for timer to work
  timer.run();
  
  // If the user presses the control button, have them set the time
  if (digitalRead(CONTROL_BUTTON)) {
	delay(DEBOUNCE);
	setTime();
  }

  // If the force coffee pin is pushed, toggle brew
  if (digitalRead(COFFEE_BUTTON)) {
    if (brewing) {
      stopBrew();
    } else {
      brew();
    }
	
	delay(DEBOUNCE);
  }
  
  if (brewing && millis() >= autoStopTime) {
    stopBrew();
  }
}


/*
 * Has the user set all the time values
 */
void setTime() {
	for (int i = 0; i < NUM_TIME_VALS; i++) {
		// Prompt user for this time val
		lcdWriteTop(timeValNames[i] + "?");

		// Show the user the current value
		if (i == MONTH) {
			lcdWriteBottom(months[timeVals[i]]);
		} else if (i == WEEKDAY) {
			lcdWriteBottom(days[timeVals[i]]);
		} else {
			lcdWriteBottom(String(timeVals[i]));
		}
		
		// Write time and adjust value as requested
		do {
			// If the up button is pressed, increment this time val
			if (digitalRead(UP_BUTTON)) {
				timeVals[i]++;
				
				// Check value and adjust as necessary
				if (i != MONTH_DAY) {
					if (timeVals[i] == maxi[i]) {
						timeVals[i] = 0;
					}
				} else {
					if (timeVals[i] > monthDays[timeVals[MONTH]]) {
						timeVals[i] = 1;
					}
				}
				
				// Write the appropriate value to show the user their changes
				if (i == MONTH) {
					lcdWriteBottom(months[timeVals[i]]);
				} else if (i == WEEKDAY) {
					lcdWriteBottom(days[timeVals[i]]);
				} else {
					lcdWriteBottom(String(timeVals[i]));
				}				
				
				// Debounce the up button
				delay(DEBOUNCE);
			// If the down button is pressed, decrement this time val
			} else if (digitalRead(DOWN_BUTTON)) {
				timeVals[i]--;
				
				// Check value and adjust as necessary
				if (i != MONTH_DAY) {
					if (timeVals[i] < 0) {
						timeVals[i] = maxi[i] - 1;
					}
				} else {
					if (timeVals[i] == 0) {
						timeVals[i] = monthDays[timeVals[MONTH]];
					}
				}
				
				// Write the appropriate value to show the user their changes
				if (i == MONTH) {
					lcdWriteBottom(months[timeVals[i]]);
				} else if (i == WEEKDAY) {
					lcdWriteBottom(days[timeVals[i]]);
				} else {
					lcdWriteBottom(String(timeVals[i]));
				}
				
				// Debounce the down button
				delay(DEBOUNCE);
			}
		} while(!digitalRead(CONTROL_BUTTON));
		
		// Debounce the control button
		delay(DEBOUNCE);
	}

	// Have this function check and display
	checkAndDisplay();
}

/*
 * Write the given text across the top of the LCD
 */
void lcdWriteTop(String text) {
	// Clear top line
	lcd.setCursor(0, 0);
	lcd.print(clearString);
	
	lcd.setCursor(0, 0);
	lcd.print(text);
}


/*
 * Write the given text across the bottom line of the LCD
 */
void lcdWriteBottom(String text) {
	// Clear top line
	lcd.setCursor(0, 1);
	lcd.print(clearString);
	
	lcd.setCursor(0, 1);
	lcd.print(text);
}


/**
 * Check if coffee should be made based on the current time
 */
void checkMakeCoffee() {
  // If the time matches for today, make coffee
  if (timeString == startTimes[timeVals[WEEKDAY]]) {
    brew();
  }
}


/**
 * Starts making coffee and displays a message
 */
void brew() {
	// Update brewing
	brewing = true;

	// Turn the relay on, turning the coffee maker on
	digitalWrite(RELAY, LOW);

	// Sound a tone signalling brewing
	tone(PIEZO, NOTE_B5, 500);

	// Set lastBrewString to the current timeString
	lastBrewString = timeString;

	// Update the brew message and display it
	brewString = "Brew Since " + lastBrewString;
	checkAndDisplay();
	
	if (autostop) {
	    autoStopTime = millis() + autoStopLength;
	}
}


/*
 * Turn off the coffee maker, update appropriate variables, and display that
 * brewing has stopped
 */
void stopBrew() {
	// Update brewing
	brewing = false;

	// Turn off the relay
	digitalWrite(RELAY, HIGH);

	// Update the brew message and display it
	brewString = "Last Brew: " + lastBrewString;
  	checkAndDisplay();
}


/**
 * Update the time and display it
 */
void updateTime() {
  // Update minute
  timeVals[MINUTE]++;
  
  // Check and display the time
  checkAndDisplay();
  
  // Check if coffee should be made
  checkMakeCoffee();
}


/**
 * Check if any time values need to be changed
 */
void checkTime() {
	// Loop backwards through the time vals
	// (Start with minute, then hour...)
	for (int i = NUM_TIME_VALS - 1; i >= 0; i--) {
		// Month days have no set max
		if (i != MONTH_DAY) {
			// If this time val is too high set it to 0 and increment the
			// appropriate next val 
			if (timeVals[i] == maxi[i]) {
				timeVals[i] = 0;
				
				if (i == MINUTE) {
					timeVals[HOUR]++;
				} else if (i == HOUR) {
					timeVals[WEEKDAY]++;
					timeVals[MONTH_DAY]++;
				}
			}
		} else {
			// If the day of the month is too high, set it to 0 and increment
			// the month
			if (timeVals[i] > monthDays[timeVals[MONTH]]) {
				timeVals[i] = 1;
				
				timeVals[MONTH]++;
			}
		}
	}
}


/*
 * Updates timeString to signify the surrent time
 */
void setTimeString() {
  // Create string versions of hour and minute
  String hourString = String(timeVals[HOUR]);
  String minuteString = String(timeVals[MINUTE]);
  
  // Add a "0" to the front of numbers under 10
  if (timeVals[HOUR] < 10) {
    hourString = "0" + hourString;
  }
  
  // Add a "0" to the front of numbers under 10
  if (timeVals[MINUTE] < 10) {
    minuteString = "0" + minuteString;
  }
  
  // Update timeString
  timeString = hourString + ":" + minuteString;
}


/**
 * Check time values, set the time string, and display the time
 */
void checkAndDisplay() {
	checkTime();
	setTimeString();
	
	lcdWriteTop(days[timeVals[WEEKDAY]] + " " 
	+ months[timeVals[MONTH]] + " " 
	+ String(timeVals[MONTH_DAY]) + " " 
	+ timeString);
	
	lcdWriteBottom(brewString);
}




