#include "status_screen.h"
#include "screen_helper.h"
#include "wifi_manager.h"
#include <TFT_eSPI.h>
#include <WiFi.h>

extern TFT_eSPI tft;

namespace {
constexpr int kHeaderHeight = 34; // Must match screen_helper
} // namespace

void drawStatusScreen() {
  renderHeader("System", 2, 3);
  clearContentArea();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  const bool connected = wifiManagerIsConnected();
  const int32_t rssi = connected ? WiFi.RSSI() : 0;
  const IPAddress ipAddr = connected ? WiFi.localIP() : IPAddress(0, 0, 0, 0);
  const String macAddr = WiFi.macAddress();
  const unsigned long uptimeMs = millis();
  const unsigned long uptimeSec = uptimeMs / 1000UL;
  const unsigned long days = uptimeSec / 86400UL;
  const unsigned long hours = (uptimeSec % 86400UL) / 3600UL;
  const unsigned long minutes = (uptimeSec % 3600UL) / 60UL;
  const unsigned long seconds = uptimeSec % 60UL;

  int y = kHeaderHeight + 5;
  tft.setTextSize(2);
  tft.setCursor(10, y);
  tft.print("Network");

  tft.setTextSize(3);
  y += 30;
  tft.setCursor(10, y);
  tft.print(connected ? "Connected" : "Offline");

  tft.setTextSize(1);
  y += 35;
  tft.setCursor(10, y);
  tft.print("Signal: ");
  if (connected) {
    tft.print(String(rssi) + " dBm");
  } else {
    tft.print("N/A");
  }

  y += 16;
  tft.setCursor(10, y);
  tft.print("IP: ");
  tft.print(ipAddr.toString());

  y += 16;
  tft.setCursor(10, y);
  tft.print("MAC: ");
  tft.print(macAddr);

  y += 16;
  tft.setCursor(10, y);
  tft.print("Uptime: ");
  tft.print(days);
  tft.print("d ");
  tft.print(hours);
  tft.print("h ");
  tft.print(minutes);
  tft.print("m ");
  tft.print(seconds);
  tft.print("s");
}

