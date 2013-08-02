/*
 * Karen v1.6
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
#define VERSION 			"Karen v1.6"

// Set pins
#define CONTROL_BUTTON		6
#define DOWN_BUTTON 		7
#define UP_BUTTON 			8
#define COFFEE_BUTTON 		9	
#define PIEZO 				10
#define RELAY 				13

// Delays for button pushing
#define DEBOUNCE 			250

// Indicies for accessing specific time vars
#define YEAR 				0
#define MONTH 				1
#define MONTH_DAY 			2
#define HOUR 				3
#define MINUTE 				4

// Constant for number of time vals
#define NUM_TIME_VALS 		5

// Indicies for the daily brew time
#define DB_HOUR 			0
#define DB_MIN  			1

// String used to clear a line of the LCD
const String clearString = "                ";

// Defaults:
// No daily brew
// Autostop
// Not brewing
// Time has not been initialized
bool dailyBrew = true;
bool autostop = true;
bool brewing = false;
bool timeInit = false;

// Set default for daily brew as 08:00
uint8_t startTime[2] = {8, 0};

// Autostop vars
uint8_t autoStopLength = 6;    // 6 minutes
uint32_t autoStopTime;

// Update vars
const uint8_t updateInterval = 60;	// 60 seconds
uint32_t updateTime = 0;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Month days have special maxi and the year has no maximum
const uint8_t maxi[5] = {0, 12, 0, 24, 60};

// Initialize time strings
String brewString = "";
String lastBrewString = "Never";


/*
 * Get everything ready
 */
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

/*
 * Continuously check to display the menu, brew coffee, or update the display
 */
void loop() {
	// If the user presses the control button, have them set the time
	if (digitalRead(CONTROL_BUTTON)) {
		delay(DEBOUNCE);
		showMenu();
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

	// If it's time to update, display the new time and check to see if it's
	// coffee time
	if (millis() >= updateTime) {
		display();
		if (dailyBrew) {
			checkMakeCoffee();
		}
		updateTime = millis() + (updateInterval * 1000);
	}
}

/*
 * Show the menu where you can choose what to change (or exit)
 */
void showMenu() {
	uint8_t index = 0;
	const uint8_t numMenus = 4; 
	String messages[numMenus] = {"Set Time", "Set Daily Brew", "Set Auto Stop", 
		"Back"};
	
	lcdWriteTop(messages[index]);
	lcdWriteBottom(clearString);
	while (!digitalRead(CONTROL_BUTTON)) {
		if (digitalRead(UP_BUTTON)) {
			if (index + 1 == numMenus) {
				index = 0;
			} else {
				index++;
			}
			lcdWriteTop(messages[index]);
			delay(DEBOUNCE);
		}

		if (digitalRead(DOWN_BUTTON)) {
			if (index == 0) {
				index = numMenus - 1;
			} else {
				index --;
			}

			lcdWriteTop(messages[index]);
			delay(DEBOUNCE);
		}
	}

	delay(DEBOUNCE);

	switch (index) {
		case 0:
			setClockTime();
			break;

		case 1:
			setDailyBrew();
			break;
		case 2:
			setAutoStop();
			break;
	}

	// Go back to clock display
	display();
}

/*
 * Has the user enabled/disable the daily brew, and set the time if it is wanted
 */
void setDailyBrew() {
	lcdWriteTop("Brew daily?");
	dailyBrew ? lcdWriteBottom("Yes") : lcdWriteBottom("No");
	while (!digitalRead(CONTROL_BUTTON)) {
		if (digitalRead(UP_BUTTON) || digitalRead(DOWN_BUTTON)) {
			dailyBrew = !dailyBrew;
			dailyBrew ? lcdWriteBottom("Yes") : lcdWriteBottom("No");
			delay(DEBOUNCE);
		}
	}

	// Debounce Control
	delay(DEBOUNCE);

	if (dailyBrew) {
		uint8_t myTimeRefs[2] = {HOUR, MINUTE};

		lcdWriteTop("Daily Brew Time?");

		for (int i = 0; i < 2; i++) {
			lcdWriteBottom(i ? makeTimeVersion(startTime[DB_HOUR]) + ":  " : 
				"  :" + makeTimeVersion(startTime[DB_MIN]));
			delay(500);
			lcdWriteBottom(makeTimeString(startTime[DB_HOUR], 
				startTime[DB_MIN]));

			while (!digitalRead(CONTROL_BUTTON)) {
				if (digitalRead(UP_BUTTON)) {
					if ((startTime[i] + 1) == maxi[myTimeRefs[i]]) {
						startTime[i] = 0;
					} else {
						startTime[i]++;
					}

					lcdWriteBottom(makeTimeString(startTime[DB_HOUR], 
						startTime[DB_MIN]));
					delay(DEBOUNCE);
				}

				if (digitalRead(DOWN_BUTTON)) {
					if (startTime[i] == 0) {
						startTime[i] = maxi[myTimeRefs[i]] - 1;
					} else {
						startTime[i]--;
					}

					lcdWriteBottom(makeTimeString(startTime[DB_HOUR], 
						startTime[DB_MIN]));
					delay(DEBOUNCE);
				}
			}

			delay(DEBOUNCE);
		}
	}
}

/*
 * Has the user set all the time values
 */
void setClockTime() {
	// Vars to make looping through time vars easier
	const String timeValNames[5] = {"Year", "Month", "Month day", "Hour", 
	 "Minute"};
	int timeVals[5];

	if (timeInit) {
		// Get time from library
		timeVals[0] = year();
		timeVals[1] = month();
		timeVals[2] = day();
		timeVals[3] = hour();
		timeVals[4] = minute();
	} else {
		// Default - Midnight on Jan 1st, 2013
		timeVals[0] = 2013;
		timeVals[1] = 1;
		timeVals[2] = 1;
		timeVals[3] = 0;
		timeVals[4] = 0;

		timeInit = true;
	}

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
		} while (!digitalRead(CONTROL_BUTTON));
		
		// Debounce the control button
		delay(DEBOUNCE);
	}

	setTime(timeVals[HOUR], timeVals[MINUTE], 0, timeVals[MONTH_DAY], 
		timeVals[MONTH], timeVals[YEAR]);
}

/*
 * Show an interface for the user to configure the autostop feature
 */
void setAutoStop() {
	lcdWriteTop("Enable Autostop?");
	lcdWriteBottom(autostop ? "Yes" : "No");

	while (!digitalRead(CONTROL_BUTTON)) {
		if (digitalRead(UP_BUTTON) || digitalRead(DOWN_BUTTON)) {
			autostop = !autostop;
			lcdWriteBottom(autostop ? "Yes" : "No");
			delay(DEBOUNCE);
		}
	}
	delay(DEBOUNCE);

	if (autostop) {
		lcdWriteTop("Autostop time");
		lcdWriteBottom(String(autoStopLength) + " min");

		while (!digitalRead(CONTROL_BUTTON)) {
			if (digitalRead(UP_BUTTON)) {
				autoStopLength++;
				lcdWriteBottom(String(autoStopLength) + " min");
				delay(DEBOUNCE);
			} 
			if(digitalRead(DOWN_BUTTON)) {
				if (!autoStopLength == 0) {
					autoStopLength--;
				}
				lcdWriteBottom(String(autoStopLength) + " min");
				delay(DEBOUNCE);
			}
		}
		delay(DEBOUNCE);
	}
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
	if (hour() == startTime[DB_HOUR] && minute() == startTime[DB_MIN]) {
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
	    autoStopTime = millis() + ((uint32_t)autoStopLength) * 60 * 1000;
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

/*
 * Creates a string for the current time
 */
String makeTimeString() {
	return makeTimeString(hour(), minute());
}

/*
 * Creates a string representing the time given
 */
String makeTimeString(uint8_t myHour, uint8_t myMinute) {
	return makeTimeVersion(myHour) + ":" + makeTimeVersion(myMinute);
}

/*
 * Returns the time acceptable version of the number
 * Ex. 7 -> "07", 14 -> "14"
 */
String makeTimeVersion(uint8_t timeUnit) {
	if (timeUnit < 10) {
		return "0" + String(timeUnit);
	} else {
		return String(timeUnit);
	}
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
