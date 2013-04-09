#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include "pitches.h"
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char serverName[] = "ruby-coffee-maker.herokuapp.com";

EthernetClient client;

// Set pins
#define PIEZO 10
#define RELAY 8


// String used to clear a line of the LCD
const String clearString = "                ";

// Initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SimpleTimer timer;

String lastBrewString = "Never";

// Initialize brewing bool
boolean brewing = false;

void setup() {
  Serial.begin(9600);
  // Start LCD
  // lcd.begin(16, 2);
  
  // Set the pin modes for peizo and relay
  pinMode(PIEZO, OUTPUT);
  pinMode(RELAY, OUTPUT);
  
  // Relay is set to LOW by default, so stop it
  stopBrew();
  
  if (Ethernet.begin(mac) == 0) {
    //lcd.setCursor(0, 0);
    //lcd.write("I have failed");
    Serial.println("ETH FAIL");
    
    while(true) {}
  }
  
  // Give the Ethernet shield a second to initialize
  delay(1000);
}

void loop() {
  // Needed for timer to work
  //timer.run();
  
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
  
  /*
  // Clear line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  // Write the start time for the coffee on line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Brew Since " + lastBrewString);
  */
}

// Turn the relay off (HIGH)
void stopBrew() {
  // Update brewing
  brewing = false;
  
  // Turn off the relay
  digitalWrite(RELAY, HIGH);
  
  Serial.println("Stop brew");
  
  /*
  // Clear line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print(clearString);
  
  // Write the time of last brew on line 2 of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Last Brew: " + lastBrewString);
  */
}


