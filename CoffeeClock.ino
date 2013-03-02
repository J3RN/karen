#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "pitches.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SimpleTimer timer;

// Set pins
int dayPin = 6;
int hourPin = 7;
int minutePin = 8;
int forceCoffeePin = 9;
int piezoPin = 10;
int relayPin = 13;

// Initialize time units
int day = 0;
int hour = 0;
int minute = 0;

// Initialize time strings
String timeString = "";
String lastBrewString = "Never";

// Initialize brewing bool
boolean brewing = false;

// Initialize days to an array of days of the week
String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
  // Start LCD
  lcd.begin(16, 2);
  
  // Set the pin modes for buttons and relay
  pinMode(dayPin, INPUT);
  pinMode(hourPin, INPUT);
  pinMode(minutePin, INPUT);
  pinMode(forceCoffeePin, INPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  // Update time every minute
  timer.setInterval(60000, updateTime);  // 60,000 milliseconds per minute
  
  // Check time and print to LCD
  checkAndDisplay();
  
  // Relay is set to LOW by default, so stop it
  stopBrew();
}

void loop() {
  // Needed for timer to work
  timer.run();
  
  // If the hour button is being pushed, increment
  // the hour and check and display the time 
  if (digitalRead(hourPin)) {
    hour++;
    checkAndDisplay();
    delay(300);
  }
  
  // If the minute button is being pushed, increment
  // the minute and check and display the time 
  if (digitalRead(minutePin)) {
    minute++;
    checkAndDisplay();
    delay(300);
  }
  
  // If the day button is being pushed, increment
  // the day and check and display the time 
  if (digitalRead(dayPin)) {
    day++;
    checkAndDisplay();
    delay(300); 
  }
  
  // If the force coffee pin is pushed, toggle brew
  if (digitalRead(forceCoffeePin)) {
    if (brewing) {
     stopBrew();
    } else {
     brew();
    }
    
    delay(300);
  }
}

/**
 * Check if coffee should be made based on the current time
 */
void checkMakeCoffee() {
  // If Sunday or Saturday at 10:00, make coffee
  if ((day == 0 || day == 6)){
    if (hour == 10 && minute == 0) {
     brew();
    }
  } else if (day == 1) {  // If the time is Monday at 9:20, make coffee
   if( hour == 9 && minute == 20) {
    brew();
   }
  } else {  // If it's any other day at 8:10, make coffee
   if (hour == 8 && minute == 10) {
     brew();
   }
  }
}

/**
 * Starts making coffee and displays a message
 */
void brew() {
  // Update brewing
  brewing = true;
  
  // Turn the relay on, turning the coffee maker on
  digitalWrite(relayPin, LOW);
  
  // Sound a tone signalling brewing
  tone(piezoPin, NOTE_B5, 500);
  
  // Set lastBrewString to the current timeString
  lastBrewString = timeString;
  
  // Clear line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  // Write the start time for the coffee on line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Brew Since " + lastBrewString);
}

void stopBrew() {
  // Update brewing
  brewing = false;
  
  // Turn off the relay
  digitalWrite(relayPin, HIGH);
  
  // Clear line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  // Write the time of last brew on line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Last Brew: " + lastBrewString);
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
  
  // If hour is 24, update day and hour
  if(hour == 24) {
    day++;
    hour = 0;
  }
  
  // If day is 7, update day
  if(day == 7) {
    day = 0;
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
 * Display the current time on the LCD
 */
void showTime() {
  // Display time on line 0 of the LCD
  lcd.setCursor(0, 0);
  lcd.print(days[day] + " - " + timeString);
}

/**
 * Check time values, set the time string, and display the time
 */
void checkAndDisplay() {
  checkTime();
  setTimeString();
  showTime();
}
