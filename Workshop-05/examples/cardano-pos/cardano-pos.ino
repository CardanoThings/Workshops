// Include necessary libraries
#include <Arduino.h>
#include <TFT_eSPI.h>

// Include our custom header files
#include "secrets.h"        // WiFi credentials (not in git)
#include "transaction_qr.h" // Transaction QR code display
#include "web_server.h"     // HTTP web server for serving files
#include "wifi_manager.h"   // WiFi connection management

// Display object - handles communication with the TFT screen
TFT_eSPI display = TFT_eSPI();

void setup() {
  // Initialize serial communication for debugging
  // Serial communication lets us send messages to the computer via USB
  // 115200 is the baud rate (speed of communication)
  // You can view these messages in the Arduino IDE Serial Monitor
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to connect

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

  // Initialize the display
  display.begin();

  // Invert display colors (useful for certain display types)
  display.invertDisplay(true);

  // Set display rotation (0 = portrait, 1-3 = other orientations)
  display.setRotation(0);

  // Fill the entire screen with black background
  display.fillScreen(TFT_BLACK);

  // Welcome Message
  display.setTextColor(TFT_WHITE);

  // Center "Cardano POS" text using MC_DATUM (Middle Center datum)
  display.setTextSize(2);
  display.setTextDatum(MC_DATUM);
  display.drawString("Cardano POS", display.width() / 2,
                     display.height() / 2 - 10);

  // Center "www.cardanothings.io" text below
  display.setTextSize(1);
  display.drawString("www.cardanothings.io", display.width() / 2,
                     display.height() / 2 + 20);
  delay(5000);
  display.fillScreen(TFT_BLACK);

  // Initialize transaction QR display
  transactionQRInit(display);

  // Register callback to display QR code when new transaction is created
  setTransactionCreatedCallback(displayNewTransactionQR, &display);
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

  // Update transaction QR display and check on-chain status
  transactionQRUpdate(display);
}
