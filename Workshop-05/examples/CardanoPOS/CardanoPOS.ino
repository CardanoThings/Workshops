// Include necessary libraries
#include <SPI.h> // Serial Peripheral Interface - for communication with TFT display
#include <TFT_eSPI.h> // TFT display library for ESP32

// Include our custom header files
#include "secrets.h"      // WiFi credentials (not in git)
#include "startscreen.h"  // Startup screen display
#include "web_server.h"   // HTTP web server for serving files
#include "wifi_manager.h" // WiFi connection management

// Create TFT display object
// This is a global object that all screen files can access
// TFT = Thin Film Transistor (the type of display technology)
TFT_eSPI tft = TFT_eSPI();

void setup() {
  // Initialize serial communication for debugging
  // Serial communication lets us send messages to the computer via USB
  // 115200 is the baud rate (speed of communication)
  // You can view these messages in the Arduino IDE Serial Monitor
  Serial.begin(115200);

  // Initialize TFT display
  // This tells the display to wake up and get ready to show graphics
  tft.init();

  // Set display rotation (1 = landscape mode)
  // Landscape means wider than tall (like a TV)
  tft.setRotation(0);

  // Invert display colors (required for some CYD displays)
  // Some displays have inverted color logic - this fixes that
  tft.invertDisplay(true);

  // Display the start screen (shows "Cardano Ticker" or similar)
  // This gives visual feedback that the device is starting up
  displayStartScreen();

  // Set up WiFi connection
  // WIFI_SSID is your WiFi network name
  // WIFI_PASSWORD is your WiFi password
  // These are defined in secrets.h (which you should create from
  // secrets.h.example)
  wifiManagerSetup(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection (with timeout)
  // We need WiFi to fetch data from the internet, so we wait here
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