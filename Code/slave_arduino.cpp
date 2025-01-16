#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define SLAVE_ADDRESS 0x08

// RFID Setup
#define RST_PIN 9      // Reset pin for MFRC522
#define SS_PIN 10      // Slave select pin for MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

volatile bool stopSignal = false;

void setup() {
  Wire.begin(SLAVE_ADDRESS); // Join I2C bus with address 0x08
  Wire.onRequest(requestEvent); // Register callback for master requests

  // RFID initialization
  SPI.begin();
  rfid.PCD_Init();

  Serial.begin(9600);
  Serial.println("RFID Reader ready...");
}

void loop() {
  // Check for RFID card
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = readRFID();
    Serial.print("Card UID: ");
    Serial.println(uid);

    if (validateRFID(uid)) {
      stopSignal = true;
    }

    rfid.PICC_HaltA(); // Stop reading the card
  }
}

// Function to read RFID UID and return it as a String
String readRFID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) {
      uid += " ";
    }
  }
  return uid;
}

// Function to validate the RFID UID
bool validateRFID(String uid) {
  const String validUIDs[] = {"B8 D5 21 12", "30 9D 7F 14"};
  for (String validUID : validUIDs) {
    if (uid.equalsIgnoreCase(validUID)) {
      Serial.println("Access Granted!");
      return true;
    }
  }
  Serial.println("Access Denied!");
  return false;
}

// Callback function to respond to master requests
void requestEvent() {
  if (stopSignal) {
    Wire.write(1); // Send STOP signal
    stopSignal = false;
  } else {
    Wire.write(0); // Send no signal
  }
}
