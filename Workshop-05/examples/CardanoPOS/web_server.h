/**
 * web_server.h - Header file for HTTP web server functionality
 * 
 * This file declares functions for managing an HTTP web server on the ESP32.
 * The server serves static files from LittleFS and provides API endpoints
 * for creating Cardano payment transactions. It handles both file serving
 * (HTML, CSS, JS, images) and JSON API requests.
 * 
 * The server runs on port 80 and must be initialized after WiFi is connected.
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

/**
 * Initialize the web server
 * 
 * Sets up LittleFS file system, configures route handlers, and starts
 * the HTTP server on port 80. This function should be called after
 * WiFi is connected.
 * 
 * The server will:
 * - Mount the LittleFS file system
 * - List all available files (for debugging)
 * - Register API endpoints for transaction creation
 * - Set up file serving for static content
 * - Start listening for HTTP requests
 */
void webServerSetup();

/**
 * Handle incoming server requests
 * 
 * Processes HTTP requests from clients. This function must be called
 * repeatedly in the main loop() to handle incoming connections.
 * The server handles requests asynchronously, so this should be called
 * frequently to maintain responsiveness.
 */
void webServerLoop();

/**
 * Check if the web server is currently running
 * 
 * @return true if the server has been started and is running, false otherwise
 */
bool webServerIsRunning();

#endif

