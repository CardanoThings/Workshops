# Web Server

The web server module provides a simple HTTP server that serves static files from LittleFS on the ESP32.

## Overview

This module handles:
- HTTP server initialization on port 80
- Serving files from LittleFS filesystem
- Request routing and file handling
- Content type detection
- Fallback to index.html for missing files

## Functions

### `webServerSetup()`

Initializes the web server. **Must be called after WiFi is connected.**

**What it does:**
1. Initializes LittleFS filesystem
2. Lists all available files (for debugging)
3. Sets up request handler for all routes
4. Starts the HTTP server on port 80

**Usage:**
```cpp
if (wifiManagerIsConnected()) {
  webServerSetup();
}
```

### `webServerLoop()`

Processes incoming HTTP requests. **Must be called regularly** in your `loop()` function.

**Usage:**
```cpp
void loop() {
  webServerLoop(); // Call regularly
  // ... other code
}
```

### `webServerIsRunning()`

Checks if the web server is currently running.

**Returns:**
- `true` if server is running
- `false` if server is not running

**Usage:**
```cpp
if (webServerIsRunning()) {
  Serial.println("Server is active");
}
```

## How It Works

### File Serving

When a client requests a file:

1. **Path Normalization**: The requested path is normalized (ensures it starts with `/`)
2. **Root Path Handling**: Requests to `/` or empty path are redirected to `/index.html`
3. **File Existence Check**: Checks if the requested file exists in LittleFS
4. **File Streaming**: If found, streams the file to the client with appropriate content type
5. **Fallback**: If file not found, tries to serve `index.html` as fallback
6. **404 Error**: If no file is found and no fallback available, returns 404 error

### Request Handling

The server uses `onNotFound()` handler to catch all requests:
- Any path requested is routed to the file handler
- This allows serving files from any path in LittleFS
- Provides a simple, file-based routing system

### Content Types

Currently, the server sets content type to `text/html` for all files. This can be extended to support:
- CSS files (`text/css`)
- JavaScript files (`application/javascript`)
- Images (`image/png`, `image/jpeg`, etc.)
- JSON (`application/json`)

## File Structure

Files in the `data/` directory are accessible at the root path:
- `data/index.html` → `http://[IP]/index.html` or `http://[IP]/`
- `data/style.css` → `http://[IP]/style.css`
- `data/app.js` → `http://[IP]/app.js`

## Key Features

- **Non-blocking**: Uses asynchronous request handling
- **File-based Routing**: Simple routing based on file paths
- **Fallback Support**: Automatically serves `index.html` for missing files
- **LittleFS Integration**: Seamlessly works with LittleFS filesystem
- **Error Handling**: Gracefully handles missing files and errors

## Requirements

- WiFi must be connected before calling `webServerSetup()`
- Files must be uploaded to LittleFS before they can be served
- Server must be initialized before calling `webServerLoop()`

## Limitations

- Currently only supports HTML content type
- No dynamic content generation (static files only)
- No authentication or security features
- Single-threaded request handling

