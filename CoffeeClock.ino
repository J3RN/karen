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

int month = 0;
int monthDay = 1;
int weekDay = 0;
int hour = 0;
int minute = 0;

boolean brewing = false;

String months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

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
  
  if (digitalRead(dayPin)) {
    delay(300);
    
    if (digitalRead(minutePin)) {
      weekDay++;
    } else if (digitalRead(hourPin)) {
      month++;
    } else {
      weekDay++;
      monthDay++; 
    }
    
    showTime();
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
  if ((weekDay == 0 || weekDay == 6)){
    if (hour == 10 && minute == 0) {
     brew();
    }
  } else if (weekDay == 1) {
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
  
  //tone(piezoPin, NOTE_B5, 1000);
  
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
    
  if (hour == 24) {
    weekDay++;
    monthDay++;
    hour = 0;
  }
    
  if (weekDay == 7) {
    weekDay = 0;
  }
  
  if (monthDay == monthDays[month]) {
    month++;
    monthDay = 0;
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
  lcd.print(days[weekDay] + " " + hourString + ":" + minuteString + " " + months[month] + " " + String(month));
}
