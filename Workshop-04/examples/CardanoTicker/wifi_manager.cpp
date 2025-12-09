/**
 * wifi_manager.cpp - WiFi connection management implementation
 *
 * This file implements WiFi connection management with automatic reconnection.
 * It stores WiFi credentials and periodically attempts to connect or reconnect
 * if the connection is lost.
 */

#include "wifi_manager.h"
#include <WiFi.h>

namespace {
// Time to wait between reconnection attempts (5 seconds)
// Prevents rapid reconnection attempts that could overwhelm the WiFi module
const unsigned long WIFI_RETRY_INTERVAL_MS = 5000;

// Maximum time to wait for a connection before retrying (12 seconds)
// If connection takes longer than this, we assume it failed and retry
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 12000;

// Stored WiFi credentials (set by wifiManagerSetup)
const char *storedSsid = nullptr;
const char *storedPassword = nullptr;

// Timestamp of the last connection attempt
// Used to implement retry intervals and connection timeouts
unsigned long lastAttemptMs = 0;

/**
 * Attempt to connect to WiFi
 *
 * Disconnects any existing connection, sets WiFi to station mode,
 * and begins connection with stored credentials.
 *
 * @param force If true, attempts connection immediately regardless of retry
 * interval
 */
void attemptConnection(bool force) {
  // Don't attempt connection if SSID is not set or empty
  if (storedSsid == nullptr || storedSsid[0] == '\0') {
    return;
  }

  const unsigned long now = millis();
  // Respect retry interval unless forced (e.g., initial setup)
  if (!force && (now - lastAttemptMs) < WIFI_RETRY_INTERVAL_MS) {
    return;
  }

  lastAttemptMs = now;

  Serial.print("WiFi: connecting to ");
  Serial.println(storedSsid);

  // Disconnect any existing connection and clear stored credentials
  WiFi.disconnect(true, true);
  // Set WiFi to station mode (client mode, not access point)
  WiFi.mode(WIFI_STA);
  // Begin connection attempt
  WiFi.begin(storedSsid, storedPassword);
}
} // namespace

/**
 * Initialize WiFi manager with credentials
 *
 * Stores the WiFi credentials and immediately attempts to connect.
 *
 * @param ssid The WiFi network name (SSID)
 * @param password The WiFi network password
 */
void wifiManagerSetup(const char *ssid, const char *password) {
  storedSsid = ssid;
  storedPassword = password;
  // Force immediate connection attempt on setup
  attemptConnection(true);
}

/**
 * Monitor and maintain WiFi connection
 *
 * Checks connection status and automatically attempts to reconnect
 * if disconnected. Uses timeout mechanism to detect failed connections.
 * Should be called repeatedly in the main loop().
 */
void wifiManagerLoop() {
  // If already connected, no action needed
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const unsigned long now = millis();
  // Check if connection attempt has timed out
  // Also handles case where no attempt has been made yet (lastAttemptMs == 0)
  const bool timedOut =
      (now - lastAttemptMs) > WIFI_CONNECT_TIMEOUT_MS || lastAttemptMs == 0;

  if (timedOut) {
    // Retry connection (respects retry interval)
    attemptConnection(false);
  }
}

/**
 * Check if WiFi is currently connected
 *
 * @return true if WiFi status is WL_CONNECTED, false otherwise
 */
bool wifiManagerIsConnected() { return WiFi.status() == WL_CONNECTED; }
