# -*- coding: utf-8 -*-
#!/usr/bin/env python

import time
import RPi.GPIO as GPIO
import httplib

GPIO.setmode(GPIO.BCM)

GPIO.setup(10, GPIO.OUT)
GPIO.setup(9, GPIO.IN)

oldSec = time.strftime("%S")

while True:
    
    print time.strftime("%a %b %d %H:%M:%S")
    
    conn = httplib.HTTPConnection("ruby-coffee-maker.herokuapp.com")
    conn.request("GET", "/coffee")
    resp = conn.getresponse()
    data = resp.read()
    
    if data == "1":
        print "Should make coffee!"
    else:
        print "Should not make coffee!"
        
    conn.close()
    
    time.sleep(1.0)
