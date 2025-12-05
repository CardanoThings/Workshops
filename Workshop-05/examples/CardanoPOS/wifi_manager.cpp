#include "wifi_manager.h"
#include <WiFi.h>

namespace {
const unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 12000;

const char *storedSsid = nullptr;
const char *storedPassword = nullptr;
unsigned long lastAttemptMs = 0;

void attemptConnection(bool force) {
  if (storedSsid == nullptr || storedSsid[0] == '\0') {
    return;
  }

  const unsigned long now = millis();
  if (!force && (now - lastAttemptMs) < WIFI_RETRY_INTERVAL_MS) {
    return;
  }

  lastAttemptMs = now;

  Serial.print("WiFi: connecting to ");
  Serial.println(storedSsid);

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(storedSsid, storedPassword);
}
} // namespace

void wifiManagerSetup(const char *ssid, const char *password) {
  storedSsid = ssid;
  storedPassword = password;
  attemptConnection(true);
}

void wifiManagerLoop() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const unsigned long now = millis();
  const bool timedOut =
      (now - lastAttemptMs) > WIFI_CONNECT_TIMEOUT_MS || lastAttemptMs == 0;

  if (timedOut) {
    attemptConnection(false);
  }
}

bool wifiManagerIsConnected() { return WiFi.status() == WL_CONNECTED; }
