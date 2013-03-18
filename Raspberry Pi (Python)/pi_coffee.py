# -*- coding: utf-8 -*-
#!/usr/bin/env python

import time
import RPi.GPIO as GPIO
import httplib

oldMin = ""
oldSec = ""

coffeeTimes = {"Sun 10:00", 
               "Mon 9:20", 
               "Tue 8:10", 
               "Wed 8:10",
               "Thu 8:10",
               "Fri 8:10"
               "Sat 10:00"}

brewPin = 10
stopPin = 9

shouldBrew = False
brewing = False

# Get GPIO components ready
GPIO.setmode(GPIO.BCM)
GPIO.setup(brewPin, GPIO.OUT)
GPIO.setup(stopPin, GPIO.IN)

# For the relay, LOW = on, HIGH = off
GPIO.output(brewPin, GPIO.HIGH)

while True:
    if time.strftime("%M") != oldMin:
        wDay = time.strftime("%a")
        month = time.strftime("%b")
        mDay = time.strftime("%d")
        timeString = time.strftime("%H:%M")
        
        if (wDay + " " + timeString) in coffeeTimes:
            shouldBrew = True
            
        oldMin = time.strftime("%M")
        
        print wDay, month, mDay, timeString
    
    if time.strftime("%S") != oldSec:
        conn = httplib.HTTPConnection("ruby-coffee-maker.herokuapp.com")
        conn.request("GET", "/coffee")
        resp = conn.getresponse()
        data = resp.read()
        
        if data == "1":
            print "Should make coffee!"
            shouldBrew = True
        else:
            print "Should not make coffee!"
        
        conn.close()
        
        oldSec = time.strftime("%S")
    
    if shouldBrew:
        GPIO.output(brewPin, GPIO.LOW)
        shouldBrew = False
        brewing = True
    
    if GPIO.input(stopPin):
        GPIO.output(brewPin, GPIO.HIGH)
        brewing = False