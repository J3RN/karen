/*
 * A script to turn on a coffee maker every morning by flipping a relay. Additionally, it displays the current 
 * time and date on an LCD screen and a piezo speaker beeps when coffee is being made.
 *
 * Author: Jonathan Arnett
 * Modified: 05/31/2013
 *
 * Pins:
 *  Control	6
 *  Up		7
 *  Down	8
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

// Set pins
#define CONTROL_BUTTON 6
#define UP_BUTTON 7
#define DOWN_BUTTON 8
#define COFFEE_BUTTON 9
#define PIEZO 10
#define RELAY 13

// Delays for button pushing
#define DOUBLE_BUTTON_PAUSE 100
#define DEBOUNCE 250

// String used to clear a line of the LCD
const String clearString = "                ";

// Initialize array of month names
const String months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Initialize array for days in each month
const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Initialize days to an array of days of the week
const String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const String startTimes[] = {"08:00", "08:00", "08:00", "08:00", "08:00", "08:00", "08:00"};

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// A timer that allows the clock to function
SimpleTimer timer;

// Initialize time units
int month = 0;
int monthDay = 1;
int weekDay = 0;
int hour = 0;
int minute = 0;

// Initialize time strings
String timeString = "";
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
  
  // Update time every minute
  timer.setInterval(60000, updateTime);  // 60,000 milliseconds per minute
  
  // Show a start-up splash for a second
  lcdWriteTop("Karen v1.4.0");
  delay(2000);

  // Have the user set the time
  setTime();

  // Check time and print to LCD
  checkAndDisplay();
  
  // Relay is set to LOW by default, so stop it
  stopBrew();
}

void loop() {
  // Needed for timer to work
  timer.run();
  
  // If the user presses the control button, have them set the time
  if (digitalRead(CONTROL_BUTTON)) {
	setTime();
  }

  // If the force coffee pin is pushed, toggle brew
  if (digitalRead(COFFEE_BUTTON)) {
    if (brewing) {
     stopBrew();
    } else {
     brew();
    }
  }

  delay(DEBOUNCE);
}


/*
 * Has the user set all the time values
 */
void setTime() {
	lcdWriteTop("Month?");
	
	do {
		if (digitalRead(UP_BUTTON)) {
			month++;
			if (month > 11) {
				month = 1;
			}
		} else if (digitalRead(DOWN_BUTTON)) {
			month--;
			if (month < 0) {
				month = 11;
			}
		}

		lcdWriteBottom(months[month]);
		
		delay(DEBOUNCE);
	} while(!digitalRead(CONTROL_BUTTON));

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
  if (timeString == startTimes[weekDay]) {
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
  
  // Write the start time for the coffee on line 2 of the LCD
  lcdWriteBottom("Brew Since " + lastBrewString);
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
  
  // Write the time of last brew on line 2 of the LCD
  lcdWriteBottom("Last Brew: " + lastBrewString);
}


/**
 * Update the time and display it
 */
void updateTime() {
  // Update minute
  minute++;
  
  // Check and display the time
  checkAndDisplay();
  
  // Check if coffee should be made
  checkMakeCoffee();
}


/**
 * Check if any time values need to be changed
 */
void checkTime() {
  // If minute is 60, update hour and minute
  if (minute == 60) {
    hour++;
    minute = 0;
  }
  
  if (hour == 24) {
    weekDay++;
    monthDay++;
    hour = 0;
  }
    
  if (weekDay == 7) {
    weekDay = 0;
  }
  
  if (monthDay > monthDays[month]) {
    month++;
    monthDay = 1;
  }
  
  if (month == 12) {
    month = 0;
  }
}


/*
 * Updates timeString to signify the surrent time
 */
void setTimeString() {
  // Create string versions of hour and minute
  String hourString = String(hour);
  String minuteString = String(minute);
  
  // Add a "0" to the front of numbers under 10
  if (hour < 10) {
    hourString = "0" + hourString;
  }
  
  // Add a "0" to the front of numbers under 10
  if (minute < 10) {
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
  lcdWriteTop(days[weekDay] + " " + months[month] + " " + String(monthDay) + " " + timeString);
}

