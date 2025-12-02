/*
 * CardanoTicker - Main Arduino Sketch
 *
 * This is the main entry point for the Cardano Ticker project. It displays
 * your Cardano wallet balance, token holdings, and NFT collection on a TFT
 * display. The device connects to WiFi and fetches data from Cardano blockchain
 * APIs (Koios, MinSwap, and Cexplorer).
 *
 * Hardware Requirements:
 * - ESP32 microcontroller (or compatible)
 * - TFT display (e.g., CYD - Cheap Yellow Display)
 * - WiFi connection
 *
 * What this project does:
 * 1. Connects to WiFi
 * 2. Fetches your Cardano wallet balance from Koios API
 * 3. Fetches your token positions from MinSwap API
 * 4. Fetches your NFT collection data from MinSwap and Cexplorer APIs
 * 5. Displays this information on rotating screens with a scrolling ticker
 */

// Include necessary libraries
#include <SPI.h> // Serial Peripheral Interface - for communication with TFT display
#include <TFT_eSPI.h> // TFT display library for ESP32

// Include our custom header files
#include "config.h"        // Configuration (API URLs, addresses)
#include "data_fetcher.h"  // Functions to fetch data from blockchain APIs
#include "datascreens.h"   // Screen drawing functions
#include "screen_helper.h" // Helper functions for screen rendering
#include "secrets.h"       // WiFi credentials (not in git)
#include "startscreen.h"   // Startup screen display
#include "ticker.h"        // Scrolling ticker at bottom of screen
#include "wifi_manager.h"  // WiFi connection management

// Create TFT display object
// This is a global object that all screen files can access
// TFT = Thin Film Transistor (the type of display technology)
TFT_eSPI tft = TFT_eSPI();

// Private namespace - variables only accessible within this file
namespace {
// How long each screen is displayed before rotating to the next (10 seconds)
const unsigned long SCREEN_DURATION_MS = 10000UL;

// Which screen is currently being displayed (0=Wallet, 1=Tokens, 2=NFTs,
// 3=Status)
uint8_t currentScreenIndex = 0;

// Timestamp of when we last changed screens (used to know when to rotate)
unsigned long lastScreenChange = 0;
} // namespace

/**
 * Displays the appropriate screen based on currentScreenIndex
 *
 * This function acts like a router - it checks which screen number we're on
 * and calls the corresponding drawing function. Think of it like changing
 * channels on a TV.
 */
void showCurrentScreen() {
  switch (currentScreenIndex) {
  case 0:
    drawWalletScreen(); // Show wallet balance screen
    break;
  case 1:
    drawTokenScreen(); // Show token holdings screen
    break;
  case 2:
    drawNFTScreen(); // Show NFT collection screen
    break;
  default:
    drawStatusScreen(); // Show system status screen (WiFi, uptime, etc.)
    break;
  }
}

/**
 * setup() - Arduino initialization function
 *
 * This function runs ONCE when the device starts up (or after reset).
 * It's like the "power-on" sequence - we set everything up here.
 *
 * In Arduino, setup() runs first, then loop() runs forever.
 */
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
  tft.setRotation(1);

  // Invert display colors (required for some CYD displays)
  // Some displays have inverted color logic - this fixes that
  tft.invertDisplay(true);

  // Display the start screen (shows "Cardano Ticker" or similar)
  // This gives visual feedback that the device is starting up
  displayStartScreen();

  Serial.println("Start screen displayed!");

  // Set up WiFi connection
  // WIFI_SSID is your WiFi network name
  // WIFI_PASSWORD is your WiFi password
  // These are defined in secrets.h (which you should create from
  // secrets.h.example)
  wifiManagerSetup(WIFI_SSID, WIFI_PASSWORD);

  // Initialize data fetcher
  // This sets all our data storage variables to zero/empty
  // Think of it as clearing a whiteboard before we start writing
  initDataFetcher();

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

  // If we successfully connected to WiFi, fetch data immediately
  if (wifiManagerIsConnected()) {
    Serial.println("WiFi connected, fetching initial data...");
    // Force immediate fetch by calling update functions
    // Normally these functions check if enough time has passed, but on startup
    // we want data right away, so we call them directly
    updateKoiosData();     // Fetch wallet balance from Koios API
    updatePortfolioData(); // Fetch tokens and NFTs from MinSwap API
  } else {
    Serial.println(
        "WiFi connection timeout - data will be fetched when connected");
    // If WiFi failed, the device will still work, it just won't have data yet
    // The loop() function will keep trying to connect
  }

  // Wait a bit before starting the rotating data screens
  // This gives the display time to settle and looks more professional
  delay(1000);

  // Initialize the scrolling ticker at the bottom of the screen
  // The ticker shows token prices scrolling horizontally
  initTicker();

  // Show the first screen (wallet screen)
  showCurrentScreen();

  // Update the ticker to show initial content
  updateTicker();

  // Record when we last changed screens (right now, since we just showed one)
  lastScreenChange = millis();
}

/**
 * loop() - Arduino main loop function
 *
 * This function runs FOREVER, over and over again, as long as the device is on.
 * Think of it like the heartbeat of your program - it keeps everything running.
 *
 * In this loop, we:
 * 1. Check/maintain WiFi connection
 * 2. Update blockchain data (but only when enough time has passed)
 * 3. Rotate between different screens every 10 seconds
 * 4. Update the scrolling ticker at the bottom
 */
void loop() {
  // Keep WiFi connection alive and check for reconnection if needed
  // This needs to be called regularly to maintain the connection
  wifiManagerLoop();

  // Update data fetchers (they handle their own timing internally)
  // These functions check internally if enough time has passed since last fetch
  // They won't fetch too often, which saves bandwidth and API rate limits
  updateKoiosData(); // Checks if 1 minute passed, then fetches wallet balance
  updatePortfolioData(); // Checks if 10 minutes passed, then fetches
                         // tokens/NFTs

  // Check if it's time to rotate to the next screen
  const unsigned long now = millis(); // Get current time
  if (now - lastScreenChange >= SCREEN_DURATION_MS) {
    // Time to switch screens!
    // The % operator gives us the remainder after division
    // This creates a cycle: 0 -> 1 -> 2 -> 3 -> 0 -> 1 -> ...
    // Example: if currentScreenIndex is 3, (3+1) % 4 = 0 (back to first screen)
    currentScreenIndex = (currentScreenIndex + 1) % TOTAL_SCREENS;

    // Draw the new screen
    showCurrentScreen();

    // Record when we changed screens so we know when to change again
    lastScreenChange = now;
  }

  // Update the scrolling ticker at the bottom of the screen
  // This needs to be called frequently to create smooth scrolling animation
  updateTicker();
}