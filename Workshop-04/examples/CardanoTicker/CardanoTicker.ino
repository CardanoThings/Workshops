#include <SPI.h>
#include <TFT_eSPI.h>

#include "config.h"
#include "data_fetcher.h"
#include "datascreens.h"
#include "screen_helper.h"
#include "secrets.h"
#include "startscreen.h"
#include "ticker.h"
#include "wifi_manager.h"

// Create TFT display object
TFT_eSPI tft = TFT_eSPI();

namespace {
const unsigned long SCREEN_DURATION_MS = 10000UL;
uint8_t currentScreenIndex = 0;
unsigned long lastScreenChange = 0;
} // namespace

void showCurrentScreen() {
  switch (currentScreenIndex) {
  case 0:
    drawWalletScreen();
    break;
  case 1:
    drawTokenScreen();
    break;
  case 2:
    drawNFTScreen();
    break;
  default:
    drawStatusScreen();
    break;
  }
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize TFT display
  tft.init();

  // Set display rotation (1 = landscape)
  tft.setRotation(1);

  // Invert display colors (required for some CYD displays)
  tft.invertDisplay(true);

  // Display the start screen
  displayStartScreen();

  Serial.println("Start screen displayed!");

  wifiManagerSetup(WIFI_SSID, WIFI_PASSWORD);

  // Initialize data fetcher
  initDataFetcher();

  // Wait for WiFi connection (with timeout)
  Serial.println("Waiting for WiFi connection...");
  const unsigned long wifiTimeout = 30000; // 30 seconds timeout
  const unsigned long wifiStart = millis();
  while (!wifiManagerIsConnected() && (millis() - wifiStart) < wifiTimeout) {
    wifiManagerLoop();
    delay(100);
  }

  if (wifiManagerIsConnected()) {
    Serial.println("WiFi connected, fetching initial data...");
    // Force immediate fetch by resetting last fetch times
    // This bypasses the interval check for initial fetch
    updateKoiosData();     // Fetch wallet balance immediately
    updatePortfolioData(); // Fetch tokens/NFTs immediately
  } else {
    Serial.println(
        "WiFi connection timeout - data will be fetched when connected");
  }

  // Wait a bit before starting the rotating data screens
  delay(1000);

  initTicker();
  showCurrentScreen();
  updateTicker();
  lastScreenChange = millis();
}

void loop() {
  wifiManagerLoop();

  // Update data fetchers (they handle their own timing)
  updateKoiosData();     // Updates every minute
  updatePortfolioData(); // Updates every 10 minutes

  const unsigned long now = millis();
  if (now - lastScreenChange >= SCREEN_DURATION_MS) {
    currentScreenIndex = (currentScreenIndex + 1) % TOTAL_SCREENS;
    showCurrentScreen();
    lastScreenChange = now;
  }
  updateTicker();
}