#include "DFRobot_EC10.h"
#include <EEPROM.h>
#include "connection.h"
#include <FirebaseESP8266.h>  // Include the Firebase library

#define EC_PIN A0
float voltage, ecValue, temperature = 25;
String lastSalinityStatus = "";  // To store the previous salinity status
DFRobot_EC10 ec;

// Create a FirebaseData object
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  ec.begin();
  connectToWiFi();
  connectToFirebase();
}

void loop() {
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U)  //time interval: 1s
  {
    timepoint = millis();
    voltage = analogRead(EC_PIN) / 1024.0 * 5000;  // read the voltage
    Serial.print("Voltage: ");
    Serial.print(voltage);
    
    // temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    ecValue = ec.readEC(voltage, temperature);  // convert voltage to EC with temperature compensation
    Serial.print(", Conductivity: ");
    Serial.print(ecValue, 1);
    Serial.print(" ms/cm");
    
    // Convert mS/cm to ppt (approximation)
    float salinity = ecValue / 1.6;
    Serial.print(", Salinity: ");
    Serial.print(salinity, 1);
    Serial.print(" ppt");
    
    // Determine if salinity is optimal, below, or above
    String salinityStatus;
    if (salinity < 27.0) {
      salinityStatus = "Below Optimal";
      Serial.println(" - Below Optimal");
    } else if (salinity > 35.0) {
      salinityStatus = "Above Optimal";
      Serial.println(" - Above Optimal");
    } else {
      salinityStatus = "Optimal";
      Serial.println(" - Optimal");
    }

    // Only update Firebase if the salinity status has changed
    if (salinityStatus != lastSalinityStatus) {
      // Update Firebase with the new salinity status
      updateFirebase(salinityStatus);  // Function to update Firebase Realtime Database
      Serial.println("Firebase updated with new salinity status.");
      
      // Store the current salinity status to compare next time
      lastSalinityStatus = salinityStatus;
    } else {
      Serial.println("Salinity status unchanged, Firebase not updated.");
    }
  }

  ec.calibration(voltage, temperature);  // calibration process by Serial CMD
}

// Function to update Firebase with new salinity status
void updateFirebase(String salinityStatus) {
  // Assuming you have a Firebase path like /Readings/Water/salinityStatus
  if (Firebase.setString(firebaseData, "/Readings/Water/salinityStatus", salinityStatus)) {
    Serial.println("Firebase update success.");
  } else {
    Serial.println("Firebase update failed: " + firebaseData.errorReason());
  }
}
