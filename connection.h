#ifndef FIREBASE_CONNECTION_H
#define FIREBASE_CONNECTION_H


#include <WiFiManager.h> // Include the WiFiManager library
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "https://smartshell-c0840-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "yWriEYoVzUbQfaj1DR9oDHUceKHVXdix27z5l8Nu"


// Forward declaration
void saveConfigCallback();

void connectToWiFi() {
  // Create an instance of WiFiManager
  WiFiManager wifiManager;
  
  
  // Set callback functions to handle saving custom parameters
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect("SmartShell");

  // Try to connect to previously saved WiFi credentials
  if (!wifiManager.autoConnect()) {
    // Reset and try again
    ESP.reset();
    delay(5000);
  }


  // If connected, print the assigned IP address
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void connectToFirebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  
}

void saveConfigCallback() {
  // Called when WiFiManager enters configuration mode to save custom parameters
  Serial.println("Configuring WiFi...");
  // You can add custom logic here to save the WiFi credentials to EEPROM or any other storage
}




#endif