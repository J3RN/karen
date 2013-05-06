#include <SimpleTimer.h>
#include "pitches.h"
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char serverName[] = "ruby-coffee-maker.herokuapp.com";

EthernetClient client;

// Set pins
#define PIEZO 2
#define RELAY 3

// Set request timeout
#define TIMEOUT 5000

// Initialize brewing bool
boolean brewing = false;

void setup() {
  Serial.begin(9600);
  
  // Set the pin modes for peizo and relay
  pinMode(PIEZO, OUTPUT);
  pinMode(RELAY, OUTPUT);
  
  // Relay is set to LOW by default, so stop it
  stopBrew();
  
  if (Ethernet.begin(mac) == 0) {-
    Serial.println("ETH FAIL");
    
    // Continue on forevermore
    while(true);
  }
  
  // Print IP address
  printIP();
  
  // Give the Ethernet shield a second to initialize
  delay(1000);
}

void loop() {
  // Check for an appropriate change  
  if (brewing) {
    checkStopCoffee();
  } else {
    checkMakeCoffee();
  }
  
  // Delay for the server's sake
  delay(1000);
}

void printIP() {
  Serial.print("My IP address: ");
  
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  
  Serial.println();
}

/**
 * Check if coffee should be made based on a request made to Heroku
 */
void checkMakeCoffee() {
  if(checkCoffeeLink("/should_brew")) {
    brew();
  }
}

// Checks Heroku to see if the coffee maker should be stopped
void checkStopCoffee() {
  if(checkCoffeeLink("/should_stop")) {
    stopBrew();
  }
}

boolean checkCoffeeLink(String link) {
  boolean should = false;
  char lastChar = '0';
  
  if (client.connect(serverName, 80)) {    
    // Make a HTTP request
    client.println("GET " + link + " HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println();
    
    // Wait for response
    unsigned long start_time = millis();
    boolean timed_out = false;
    while (!client.available() && !timed_out) {
      if (millis() - start_time >= TIMEOUT) {
        timed_out = true;
      }
    }
 
    if(!timed_out) {
      while (client.available()) {
        char c = client.read();
        lastChar = c;
      }
    } else {
      Serial.println("Timed out");
    }
    
    client.stop();
  } else {
    Serial.println("Connection to server failure");
  }
  
  // If should stop brewing, stop brewing
  if (lastChar == '1') {
    should = true;
  } else if (lastChar != '0') {
    Serial.println("Invalid data");
    printIP();
  }
  
  return should;
}

/**
 * Starts making coffee and displays a message
 */
void brew() {
  // Update brewing
  brewing = true;
  
  // Turn the relay on, turning the coffee maker on
  digitalWrite(RELAY, LOW);
  
  // Sound a tone signalling brewing
  tone(PIEZO, NOTE_B5, 500);
  
  Serial.println("Brew");
}

// Turn the relay off (HIGH)
void stopBrew() {
  // Update brewing
  brewing = false;
  
  // Turn off the relay
  digitalWrite(RELAY, HIGH);
  
  Serial.println("Stop brew");
}


