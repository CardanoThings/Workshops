// Include necessary libraries
#include <Arduino.h>

// Include our custom header files
#include "secrets.h"      // WiFi credentials (not in git)
#include "web_server.h"   // HTTP web server for serving files
#include "wifi_manager.h" // WiFi connection management

void setup() {
  // Initialize serial communication for debugging
  // Serial communication lets us send messages to the computer via USB
  // 115200 is the baud rate (speed of communication)
  // You can view these messages in the Arduino IDE Serial Monitor
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to connect

  Serial.println("Basic Web Server Example");
  Serial.println("========================");

  // Set up WiFi connection
  // WIFI_SSID is your WiFi network name
  // WIFI_PASSWORD is your WiFi password
  // These are defined in secrets.h (which you should create from
  // secrets.h.example)
  wifiManagerSetup(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection (with timeout)
  // We need WiFi to serve web pages, so we wait here
  Serial.println("Waiting for WiFi connection...");
  const unsigned long wifiTimeout =
      30000; // 30 seconds timeout (in milliseconds)
  const unsigned long wifiStart = millis(); // Record when we started waiting

  // Keep checking if WiFi is connected, but don't wait forever
  // millis() returns the number of milliseconds since the device started
  while (!wifiManagerIsConnected() && (millis() - wifiStart) < wifiTimeout) {
    wifiManagerLoop(); // Check WiFi status and try to connect
    delay(100);        // Wait 100ms before checking again (don't waste CPU)
  }

  // Start web server if WiFi is connected
  if (wifiManagerIsConnected()) {
    webServerSetup();
  } else {
    Serial.println("WiFi connection failed - web server not started");
  }
}

void loop() {
  // Keep WiFi connection alive and check for reconnection if needed
  // This needs to be called regularly to maintain the connection
  wifiManagerLoop();

  // Handle web server requests (runs asynchronously, but we call loop for
  // consistency)
  if (wifiManagerIsConnected() && !webServerIsRunning()) {
    // If WiFi just reconnected, start the server
    webServerSetup();
  }
  webServerLoop();
}
