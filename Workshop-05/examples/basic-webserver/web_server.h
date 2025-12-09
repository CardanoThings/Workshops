#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

// Initialize the web server (call after WiFi is connected)
void webServerSetup();

// Handle server requests (call in loop())
void webServerLoop();

// Check if server is running
bool webServerIsRunning();

#endif

