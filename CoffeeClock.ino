#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "pitches.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SimpleTimer timer;

int relayPin = 6;

int forceCoffeePin = 7;

int piezoPin = 8;

int minutePin = 9;
int hourPin = 10;

int day = 0;
int hour = 0;
int minute = 0;

boolean brewing = false;

String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  lcd.begin(16, 2);
  timer.setInterval(60000, updateTime);  // 60,000 milliseconds per minute
  showTime();
  
  pinMode(hourPin, INPUT);
  pinMode(minutePin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  stopBrew();
}

void loop() {
  timer.run();
  
  if (digitalRead(hourPin)) {
    hour++;
    showTime();
    delay(300);
  }
  
  if (digitalRead(minutePin)) {
    minute++;
    showTime();
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
  
  tone(piezoPin, NOTE_B5, 1000);
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Brewing coffee");
}

void stopBrew() {
  brewing = false;
  
  digitalWrite(relayPin, HIGH);
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Not brewing");
}

void updateTime() {
  minute++;
  
  showTime();
  
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

void showTime() {
  checkTime();
  
  String hourString = String(hour);
  String minuteString = String(minute);
  
  if (hour < 10) {
    hourString = "0" + hourString;
  }
  
  if (minute < 10) {
    minuteString = "0" + minuteString;
  }
  
  lcd.setCursor(0, 0);
  lcd.print(days[day] + " - " + hourString + ":" + minuteString);
}
