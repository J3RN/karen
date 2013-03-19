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
shouldStop = False
brewing = False

# Get GPIO components ready
GPIO.setmode(GPIO.BCM)
GPIO.setup(brewPin, GPIO.OUT)
GPIO.setup(stopPin, GPIO.IN)

# Turn off those hideously annoying warnings
GPIO.setwarnings(False)

# Force the relay to "off." It is "on" by default sometimes
GPIO.output(brewPin, GPIO.LOW)

# Display "Not brewing" by default
lcd.setCursor(0, 1)
lcd.message("Not brewing")

# A string to hold states
lcdTime = ""
brewString = "Not brewing"

def updateLCD():
    lcd.clear()
    lcd.message(lcdTime + "\n" + brewString)

while True:
    # Every minute
    if time.strftime("%M") != oldMin:
        # Update the time to display and make a string
        # for comparison with the morning brew times
        lcdTime = time.strftime("%a %b %d %H:%M")
        compareTime = time.strftime("%a %H:%M")
        
        # Check if morning coffee should be brewed
        if compareTime in coffeeTimes:
            shouldBrew = True
        
        # Reset oldMin
        oldMin = time.strftime("%M")
        
        # Show the new time
        updateLCD()
    
    # Every second
    if time.strftime("%S") != oldSec:
        # Connect to the app website
        conn = httplib.HTTPConnection("ruby-coffee-maker.herokuapp.com")
        
        # If coffee is brewing, check if it should start
        if not brewing:
            conn.request("GET", "/should_brew")
            resp = conn.getresponse()
            data = resp.read()
            
            if data == "1":
                shouldBrew = True
                
        # If coffee is brewing, check if it should stop
        else:
            conn.request("GET", "/should_stop")
            resp = conn.getresponse()
            data = resp.read()
            
            if data == "1":
                shouldStop = True
        
        # Close connection to website
        conn.close()
        
        #update oldSec
        oldSec = time.strftime("%S")
    
    if shouldBrew:
        GPIO.output(brewPin, GPIO.HIGH)
        
        shouldBrew = False
        brewing = True
        
        brewString = "Brewing"
        updateLCD()
    
    if GPIO.input(stopPin) or shouldStop:
        GPIO.output(brewPin, GPIO.LOW)
        
        brewing = False
        shouldStop = False        
        
        brewString = "Not brewing"
        updateLCD()
            