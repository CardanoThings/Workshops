# Basic Webserver

This is the main Arduino sketch that ties together the WiFi manager and web server components to create a simple HTTP server on your ESP32.

## Overview

The sketch demonstrates how to:
- Connect to WiFi using the WiFi manager
- Initialize and start a web server
- Serve static files from LittleFS
- Handle WiFi reconnection automatically

## How It Works

### Setup Function

1. **Serial Initialization**: Starts serial communication at 115200 baud for debugging
2. **WiFi Setup**: Calls `wifiManagerSetup()` with your WiFi credentials from `secrets.h`
3. **Wait for Connection**: Waits up to 30 seconds for WiFi to connect, checking every 100ms
4. **Start Server**: If WiFi connects successfully, calls `webServerSetup()` to initialize the web server

### Loop Function

The main loop continuously:
- **Maintains WiFi**: Calls `wifiManagerLoop()` to check connection status and reconnect if needed
- **Handles Requests**: Calls `webServerLoop()` to process incoming HTTP requests
- **Auto-Restart Server**: If WiFi reconnects after being disconnected, automatically restarts the web server

## Key Concepts

- **Non-blocking**: The WiFi manager and web server use non-blocking code, so your microcontroller can do other tasks
- **Automatic Reconnection**: If WiFi disconnects, the system automatically attempts to reconnect
- **File Serving**: The web server serves files from LittleFS, which must be uploaded separately

## Usage

1. Copy `secrets.h.example` to `secrets.h` and add your WiFi credentials
2. Upload files to LittleFS using Arduino IDE's "ESP32 Sketch Data Upload" tool
3. Upload the sketch to your ESP32
4. Open Serial Monitor (115200 baud) to see connection status
5. Access the web server at `http://[ESP32_IP_ADDRESS]`

## Dependencies

- `secrets.h` - WiFi credentials (create from `secrets.h.example`)
- `wifi_manager.h` / `wifi_manager.cpp` - WiFi connection management (see `wifi_manager.md` for detailed documentation)
- `web_server.h` / `web_server.cpp` - HTTP web server implementation (see `web_server.md` for detailed documentation)

