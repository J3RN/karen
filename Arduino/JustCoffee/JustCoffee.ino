/*
 * Karen v1.4.3
 *
 * A script to turn on a coffee maker every morning by flipping a relay. 
 * Additionally, it displays the current time and date on an LCD screen and a 
 * piezo speaker beeps when it is assumed that the coffee is finished.
 *
 * Author: Jonathan Arnett
 * Modified: 06/24/2013
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

#include <Time.h>
#include <LiquidCrystal.h>
#include "pitches.h"

// Define version number
#define VERSION "Karen v1.4.4"

// Set pins
#define CONTROL_BUTTON 6
#define DOWN_BUTTON 7
#define UP_BUTTON 8
#define COFFEE_BUTTON 9
#define PIEZO 10
#define RELAY 13

// Delays for button pushing
#define DEBOUNCE 250

// Indicies for accessing specific time vars
#define YEAR 		0
#define MONTH 		1
#define MONTH_DAY 	2
#define HOUR 		3
#define MINUTE 		4

// Constant for number of time vals
#define NUM_TIME_VALS 5

// String used to clear a line of the LCD
const String clearString = "                ";

// 7:00 every morning
const uint8_t startHours[7] = {7, 7, 7, 7, 7, 7, 7};
const uint8_t startMinutes[7] = {0, 0, 0, 0, 0, 0, 0};

// Have the coffee pot turn off after a given set of time
const bool autostop = true;

// Time (in milliseconds) for the coffee pot to turn off
const uint32_t autoStopLength = 360000;    // 6 minutes

// Time to stop brewing if autostop is enabled
uint32_t autoStopTime;

// Display update interval
uint16_t updateInterval = 30000;	// 30 seconds

// Next time for display to be updated
uint32_t updateTime = 0;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Vars to make looping through time vars easier
const String timeValNames[5] = {"Year", "Month", "Month day", "Hour", 
 "Minute"};

// Month days have special maxi and the year has no maximum
const int maxi[5] = {0, 12, 0, 24, 60};

// Default - Midnight on Jan 1st, 2013
int timeVals[5] = {2013, 1, 1, 0, 0};

// Initialize time strings
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
  	setClockTime();
}

void loop() {
	// If the user presses the control button, have them set the time
	if (digitalRead(CONTROL_BUTTON)) {
		delay(DEBOUNCE);
		setClockTime();
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

	// If the user has enabled autostop and it is autostop time, stop brewing 
	// and beep
	if (brewing && millis() >= autoStopTime) {
		stopBrew();
		tone(PIEZO, NOTE_A4, 1000);
	}

	if (millis() >= updateTime) {
		display();
		updateTime = millis() + updateInterval;
	}
}

/*
 * Has the user set all the time values
 */
void setClockTime() {
	for (int i = 0; i < NUM_TIME_VALS; i++) {
		// Prompt user for this time val
		lcdWriteTop(timeValNames[i] + "?");

		// Show the user the current value
		if (i == MONTH) {
			lcdWriteBottom(monthShortStr(timeVals[i]));
		} else {
			lcdWriteBottom(String(timeVals[i]));
		}
		
		// Write time and adjust value as requested
		do {
			// If the up button is pressed, increment this time val
			if (digitalRead(UP_BUTTON)) {
				timeVals[i]++;
				
				// Check value and adjust as necessary
				if (i == HOUR || i == MINUTE) {
					if (timeVals[i] == maxi[i]) {
						timeVals[i] = 0;
					}
				} else if (i == MONTH) {
					if (timeVals[i] > maxi[i]) {
						timeVals[i] = 1;
					}
				} else if (i == MONTH_DAY) {
					if (timeVals[i] > monthDays[timeVals[MONTH] - 1]) {
						timeVals[i] = 1;
					}
				}
				
				// Write the appropriate value to show the user their changes
				if (i == MONTH) {
					lcdWriteBottom(monthShortStr(timeVals[i]));
				} else {
					lcdWriteBottom(String(timeVals[i]));
				}				
				
				// Debounce the up button
				delay(DEBOUNCE);
			// If the down button is pressed, decrement this time val
			} else if (digitalRead(DOWN_BUTTON)) {
				timeVals[i]--;
				
				// Check value and adjust as necessary
				if (i == HOUR || i == MINUTE) {
					if (timeVals[i] < 0) {
						timeVals[i] = maxi[i] - 1;
					}
				} else if (i == MONTH) {
					if (timeVals[i] == 0) {
						timeVals[i] = maxi[i];
					}
				} else if (i == MONTH_DAY) {
					if (timeVals[i] == 0) {
						timeVals[i] = monthDays[timeVals[MONTH] - 1];
					}
				} else if (i == YEAR) {
					// Maintain 0
					if (timeVals[i] == 0) {
						timeVals[i]++;
					}
				}
				
				// Write the appropriate value to show the user their changes
				if (i == MONTH) {
					lcdWriteBottom(monthShortStr(timeVals[i]));
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

	setTime(timeVals[HOUR], timeVals[MINUTE], 0, timeVals[MONTH_DAY], 
		timeVals[MONTH], timeVals[YEAR]);

	// Go to normal display
	display();
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
	uint8_t today = weekday();
	if (hour() == startHours[today] && minute() == startMinutes[today]) {
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

	// Set lastBrewString to the current timeString
	lastBrewString = makeTimeString();

	// Update the brew message and display it
	brewString = "Brew Since " + lastBrewString;
	display();
	
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
  	display();
}

String makeTimeString() {
	String hourString, minString;
	uint16_t curHour = hour(), curMin = minute();

	if (curHour < 10) {
		hourString = "0" + String(curHour);
	} else {
		hourString = String(curHour);
	}

	if (curMin < 10) {
		minString = "0" + String(minString);
	} else {
		minString = String(minString);
	}

	return hourString + ":" + minString;
}

/**
 * Check time values, set the time string, and display the time
 */
void display() {
	

	lcdWriteTop(
	 String(dayShortStr(weekday())) + " " +
	 String(monthShortStr(month())) + " " +
	 String(day()) + " " +
	 makeTimeString()
	 );
	
	lcdWriteBottom(brewString);
}
