#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

void wifiManagerSetup(const char *ssid, const char *password);
void wifiManagerLoop();
bool wifiManagerIsConnected();

#endif


