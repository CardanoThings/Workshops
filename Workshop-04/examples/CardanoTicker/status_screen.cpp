/**
 * status_screen.cpp - System Status Screen
 * 
 * This screen displays device and network information:
 * - WiFi connection status
 * - WiFi signal strength (RSSI)
 * - IP address (your device's address on the network)
 * - MAC address (unique hardware identifier)
 * - Uptime (how long the device has been running)
 * 
 * This is useful for debugging connection issues and monitoring device health.
 */

#include "status_screen.h"
#include "screen_helper.h"
#include "wifi_manager.h"
#include <TFT_eSPI.h>
#include <WiFi.h>

// External reference to TFT display
extern TFT_eSPI tft;

/**
 * Draw the system status screen
 * 
 * This is the last screen (index 3). It shows technical information about
 * the device and network connection.
 */
void drawStatusScreen() {
  // Draw header with title and page indicator
  // activeIndex = 3 means this is the fourth (last) screen
  renderHeader("System", 3);
  
  // Clear the content area
  clearContentArea();

  // Set default text color
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Gather all status information
  const bool connected = wifiManagerIsConnected();  // Is WiFi connected?
  const int32_t rssi = connected ? WiFi.RSSI() : 0;  // Signal strength (only if connected)
  const IPAddress ipAddr = connected ? WiFi.localIP() : IPAddress(0, 0, 0, 0);  // IP address
  const String macAddr = WiFi.macAddress();  // MAC address (always available)
  
  // Calculate uptime (how long device has been running)
  const unsigned long uptimeMs = millis();  // Milliseconds since startup
  const unsigned long uptimeSec = uptimeMs / 1000UL;  // Convert to seconds
  
  // Break down uptime into days, hours, minutes, seconds
  const unsigned long days = uptimeSec / 86400UL;  // 86400 seconds = 1 day
  const unsigned long hours = (uptimeSec % 86400UL) / 3600UL;  // Remaining hours
  const unsigned long minutes = (uptimeSec % 3600UL) / 60UL;  // Remaining minutes
  const unsigned long seconds = uptimeSec % 60UL;  // Remaining seconds

  // Start drawing below header
  int y = kHeaderHeight + 5;
  
  // Draw "Network" label
  tft.setTextSize(2);
  tft.setCursor(10, y);
  tft.print("Network");

  // Draw connection status in large text
  tft.setTextSize(3);  // Large text
  y += 30;
  tft.setCursor(10, y);
  tft.print(connected ? "Connected" : "Offline");  // Show status

  // Draw WiFi signal strength
  tft.setTextSize(1);  // Small text
  y += 35;
  tft.setCursor(10, y);
  tft.print("Signal: ");
  if (connected) {
    // RSSI = Received Signal Strength Indicator
    // Measured in dBm (decibels relative to milliwatt)
    // Typical range: -30 (excellent) to -90 (poor)
    // Negative numbers are normal - closer to 0 is better
    tft.print(String(rssi) + " dBm");
  } else {
    tft.print("N/A");  // Not available if not connected
  }

  // Draw IP address
  y += 16;
  tft.setCursor(10, y);
  tft.print("IP: ");
  // IP address = Internet Protocol address
  // This is your device's address on your local network
  // Format: XXX.XXX.XXX.XXX (e.g., 192.168.1.100)
  tft.print(ipAddr.toString());

  // Draw MAC address
  y += 16;
  tft.setCursor(10, y);
  tft.print("MAC: ");
  // MAC address = Media Access Control address
  // This is a unique identifier for your device's network hardware
  // Format: XX:XX:XX:XX:XX:XX (e.g., AA:BB:CC:DD:EE:FF)
  // Unlike IP address, MAC address never changes
  tft.print(macAddr);

  // Draw uptime
  y += 16;
  tft.setCursor(10, y);
  tft.print("Uptime: ");
  // Display uptime in human-readable format: "Xd Xh Xm Xs"
  tft.print(days);
  tft.print("d ");  // Days
  tft.print(hours);
  tft.print("h ");  // Hours
  tft.print(minutes);
  tft.print("m ");  // Minutes
  tft.print(seconds);
  tft.print("s");   // Seconds
}

