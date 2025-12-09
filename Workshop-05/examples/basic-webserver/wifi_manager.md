# WiFi Manager

The WiFi manager provides a robust, non-blocking solution for managing WiFi connections on the ESP32 with automatic reconnection capabilities.

## Overview

This module handles:
- WiFi connection initialization
- Connection status monitoring
- Automatic reconnection on disconnect
- Connection timeout handling

## Functions

### `wifiManagerSetup(ssid, password)`

Initializes the WiFi manager with your network credentials and immediately attempts to connect.

**Parameters:**
- `ssid`: Your WiFi network name (SSID)
- `password`: Your WiFi network password

**Usage:**
```cpp
wifiManagerSetup("MyNetwork", "MyPassword");
```

### `wifiManagerLoop()`

Monitors the WiFi connection and attempts to reconnect if disconnected. **Must be called regularly** in your `loop()` function.

**Features:**
- Checks connection status every call
- Automatically retries connection if disconnected
- Respects retry intervals to prevent overwhelming the WiFi module
- Uses timeout mechanism to detect failed connections

**Usage:**
```cpp
void loop() {
  wifiManagerLoop(); // Call regularly
  // ... other code
}
```

### `wifiManagerIsConnected()`

Checks if WiFi is currently connected.

**Returns:**
- `true` if connected to WiFi
- `false` if disconnected

**Usage:**
```cpp
if (wifiManagerIsConnected()) {
  Serial.println("WiFi is connected!");
}
```

## How It Works

### Connection Management

The WiFi manager uses several mechanisms to ensure reliable connectivity:

1. **Retry Interval**: Waits 5 seconds between reconnection attempts to prevent overwhelming the WiFi module
2. **Connection Timeout**: If a connection attempt takes longer than 12 seconds, it's considered failed and retried
3. **Station Mode**: Always operates in WiFi Station (client) mode, not Access Point mode

### State Management

- **Stored Credentials**: WiFi credentials are stored when `wifiManagerSetup()` is called
- **Last Attempt Tracking**: Tracks when the last connection attempt was made to implement retry intervals
- **Automatic Retry**: If disconnected, automatically attempts to reconnect after the timeout period

## Key Features

- **Non-blocking**: Never blocks execution, allowing your code to continue running
- **Automatic Reconnection**: Handles WiFi disconnections automatically
- **Timeout Protection**: Prevents hanging on failed connection attempts
- **Retry Throttling**: Prevents rapid reconnection attempts that could cause issues

## Error Handling

The WiFi manager handles various error scenarios:
- Invalid or empty SSID
- Connection timeouts
- Network unavailability
- WiFi module issues

All errors are handled gracefully without crashing your program.

