# -*- coding: utf-8 -*-
#!/usr/bin/env python

import time
import RPi.GPIO as GPIO
import httplib
import CharLCD

oldMin = ""
oldSec = ""

coffeeTimes = {"Sun 10:00", 
               "Mon 9:20", 
               "Tue 8:10", 
               "Wed 8:10",
               "Thu 8:10",
               "Fri 8:10"
               "Sat 10:00"}

lcd = CharLCD.CharLCD()
lcd.begin(16, 2)

brewPin = 14
stopPin = 9

shouldBrew = False
brewing = False

# Get GPIO components ready
GPIO.setmode(GPIO.BCM)
GPIO.setup(brewPin, GPIO.OUT)
GPIO.setup(stopPin, GPIO.IN)

# Turn off those hideously annoying warnings
GPIO.setwarnings(False)

# Display "Not brewing" by default
lcd.setCursor(0, 1)
lcd.message("Not brewing")

while True:
    if time.strftime("%M") != oldMin:        
        lcdTime = time.strftime("%a %b %d %H:%M")
        compareTime = time.strftime("%a %H:%M")
        
        if compareTime in coffeeTimes:
            shouldBrew = True
            
        oldMin = time.strftime("%M")
        
        lcd.setCursor(0, 0)
        lcd.message(lcdTime)
    
    if time.strftime("%S") != oldSec:
        conn = httplib.HTTPConnection("ruby-coffee-maker.herokuapp.com")
        conn.request("GET", "/should_brew")
        resp = conn.getresponse()
        data = resp.read()
        
        if data == "1":
            shouldBrew = True
        
        conn.close()
        
        oldSec = time.strftime("%S")
    
    if shouldBrew:
        GPIO.output(brewPin, GPIO.HIGH)
        shouldBrew = False
        brewing = True
        lcd.setCursor(0, 1)
        lcd.message("Brewing    ")
    
    if GPIO.input(stopPin):
        GPIO.output(brewPin, GPIO.LOW)
        brewing = False
        lcd.setCursor(0, 1)
        lcd.message("Not brewing")
            