/**
 * wifi_manager.h - Header file for WiFi connection management
 *
 * This file declares functions for managing WiFi connectivity on the ESP32.
 * It handles connection setup, connection monitoring, and provides status
 * information about the WiFi connection state.
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

/**
 * Initialize WiFi connection
 *
 * Sets up WiFi with the provided credentials and attempts to connect.
 * Call this once in setup() before using other WiFi functions.
 *
 * @param ssid The WiFi network name (SSID)
 * @param password The WiFi network password
 */
void wifiManagerSetup(const char *ssid, const char *password);

/**
 * Update WiFi connection status
 *
 * Monitors the WiFi connection and attempts to reconnect if disconnected.
 * Call this repeatedly in loop() to maintain connection.
 */
void wifiManagerLoop();

/**
 * Check if WiFi is currently connected
 *
 * @return true if connected to WiFi, false otherwise
 */
bool wifiManagerIsConnected();

#endif
