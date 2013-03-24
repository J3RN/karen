# -*- coding: utf-8 -*-
#!/usr/bin/env python

from time import sleep
from time import strftime
import RPi.GPIO as GPIO
import httplib
import CharLCD

# Errors
from httplib import BadStatusLine
from httplib import IncompleteRead
from socket import gaierror

# Start up LCD
lcd = CharLCD.CharLCD()
lcd.begin(16, 2)

# Set pins
brewPin = 14

# Not brewing by default
brewing = False

# Get GPIO components ready
GPIO.setmode(GPIO.BCM)
GPIO.setup(brewPin, GPIO.OUT)

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
oldMin = ""
oldSec = ""

brew = GPIO.LOW
stopBrew = GPIO.HIGH

# Functions
def update_LCD():
    lcd.clear()
    lcd.message(lcdTime + "\n" + brewString)
    
def start_brewing():
    # Mod global vars, not create local ones
    global brewing, brewString    
    
    GPIO.output(brewPin, brew)
    
    brewing = True
    
    brewString = "Brewing"
    update_LCD()
    
def stop_brewing():
    # Mod global vars, not create local ones
    global brewing, brewString    
    
    GPIO.output(brewPin, stopBrew)
            
    brewing = False     
    
    brewString = "Not brewing"
    update_LCD()

def get_web_data(url):
     # Connect to the app website
    conn = httplib.HTTPConnection("ruby-coffee-maker.herokuapp.com")
    
    data = ""    
    
    try:
        conn.request("GET", url)
        resp = conn.getresponse()
        data = resp.read()
    except BadStatusLine:
        print "Bad Status Line " + lcdTime;
    except gaierror:
        print "GAIERROR" + lcdTime;
    except IncompleteRead:
        print "Incomplete Read" + lcdTime;
        
    # Close connection to website
    conn.close()
    
    return data;
    
lcd.clear()
lcd.message("Karen 1.1");
sleep(1.0)

# Main program
while True:        
    # If coffee is brewing, check if it should start
    if not brewing:
        data = get_web_data("/should_brew")
        
        if data == "1":
            start_brewing()
        
    # If coffee is brewing, check if it should stop
    else:
        data = get_web_data("/should_stop")
        
        if data == "1":
            stop_brewing()
    
    # Every minute
    if strftime("%M") != oldMin:
        # Update the time to display and make a string
        # for comparison with the morning brew times
        lcdTime = strftime("%a %b %d %H:%M")

        # Show the new time
        update_LCD()
        
        # Reset oldMin
        oldMin = strftime("%M")
    
    # Sleep to decrease processor load
    sleep(1.0)
    