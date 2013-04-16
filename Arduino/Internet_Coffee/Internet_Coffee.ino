#include <SimpleTimer.h>
#include "pitches.h"
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char serverName[] = "ruby-coffee-maker.herokuapp.com";

EthernetClient client;

// Set pins
#define PIEZO 10
#define RELAY 8

// Initialize brewing bool
boolean brewing = false;

void setup() {
  Serial.begin(9600);
  
  // Set the pin modes for peizo and relay
  pinMode(PIEZO, OUTPUT);
  pinMode(RELAY, OUTPUT);
  
  // Relay is set to LOW by default, so stop it
  stopBrew();
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("ETH FAIL");
    
    // Continue on forevermore
    while(true) {}
  }
  
  // Print IP address
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
  
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

/**
 * Check if coffee should be made based on a request made to Heroku
 */
void checkMakeCoffee() {
  // Sends a request to Heroku
  char lastChar = '0';
  
  if (client.connect(serverName, 80)) {    
    // Make a HTTP request
    client.println("GET /should_brew HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println();
    
    // Wait for response
    while (!client.available());

    while (client.available()) {
      char c = client.read();
      lastChar = c;
    }
    
    client.stop();
  }
  
  // If should brew, brew
  if (lastChar == '1') {
    brew();
  } else if (lastChar != '0') {
    Serial.println("Connection failure");
    while(true);
  }
}

// Checks Heroku to see if the coffee maker should be stopped
void checkStopCoffee() {
  // Sends a request to Heroku
  char lastChar = '0';
  
  if (client.connect(serverName, 80)) {    
    // Make a HTTP request
    client.println("GET /should_stop HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println();
    
    // Wait for response
    while (!client.available());

    while (client.available()) {
      char c = client.read();
      lastChar = c;
    }
    
    client.stop();
  }
  
  // If should stop brewing, stop brewing
  if (lastChar == '1') {
    stopBrew();
  } else if (lastChar != '0') {
    Serial.println("Connection failure");
    while(true);
  }
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


