#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "pitches.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SimpleTimer timer;

int dayPin = 6;
int hourPin = 7;
int minutePin = 8;
int forceCoffeePin = 9;
int relayPin = 13;

int day = 0;
int hour = 0;
int minute = 0;

String timeString = "";
String lastBrewString = "Never";

boolean brewing = false;

String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
  lcd.begin(16, 2);
  timer.setInterval(60000, updateTime);  // 60,000 milliseconds per minute
  checkAndDisplay();
  
  pinMode(hourPin, INPUT);
  pinMode(minutePin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  stopBrew();
}

void loop() {
  timer.run();
  
  if (digitalRead(hourPin)) {
    hour++;
    checkAndDisplay();
    delay(300);
  }
  
  if (digitalRead(minutePin)) {
    minute++;
    checkAndDisplay();
    delay(300);
  }
  
  if (digitalRead(dayPin)) {
    day++;
    checkAndDisplay();
    delay(300); 
  }
  
  if (digitalRead(forceCoffeePin)) {
    if (brewing) {
     stopBrew();
    } else {
     brew();
    }
    
    delay(300);
  }
}

void checkMakeCoffee() {
  if ((day == 0 || day == 6)){
    if (hour == 10 && minute == 0) {
     brew();
    }
  } else if (day == 1) {
   if( hour == 9 && minute == 20) {
    brew();
   }
  } else {
   if (hour == 8 && minute == 10) {
     brew();
   }
  }
}

void brew() {
  brewing = true;
  
  digitalWrite(relayPin, LOW);
  
  //tone(piezoPin, NOTE_B5, 500);
  
  lastBrewString = timeString;
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Brew Since " + lastBrewString);
}

void stopBrew() {
  brewing = false;
  
  digitalWrite(relayPin, HIGH);
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Last Brew: " + lastBrewString);
}

void updateTime() {
  minute++;
  
  checkAndDisplay();
  
  checkMakeCoffee();
}

void checkTime() {
  if (minute == 60) {
    hour++;
    minute = 0;
  }
    
  if(hour == 24) {
    day++;
    hour = 0;
  }
    
  if(day == 7) {
    day = 0;
  } 
}

void setTimeString() {
  String hourString = String(hour);
  String minuteString = String(minute);
  
  if (hour < 10) {
    hourString = "0" + hourString;
  }
  
  if (minute < 10) {
    minuteString = "0" + minuteString;
  }
  
  timeString = hourString + ":" + minuteString;
}

void showTime() {
  lcd.setCursor(0, 0);
  lcd.print(days[day] + " - " + timeString);
}

void checkAndDisplay() {
  checkTime();
  setTimeString();
  showTime();
}
