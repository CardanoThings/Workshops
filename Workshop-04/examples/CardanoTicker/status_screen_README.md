# Status Screen

The status screen shows technical information about your device and network connection. This is useful for debugging and monitoring. It's the last screen (index 3) in the rotating display.

## What It Shows

- **Network Status**: "Connected" or "Offline" in large text
- **Signal Strength**: WiFi signal strength in dBm (closer to 0 is better)
- **IP Address**: Your device's address on the network (e.g., 192.168.1.100)
- **MAC Address**: Your device's unique hardware identifier
- **Uptime**: How long the device has been running (e.g., "2d 5h 30m 15s")

## How It Works

The uptime is calculated using `millis()` and converted to days, hours, minutes, and seconds. This is the same timing technique you've been using throughout the workshops!

The function gathers all status information first: WiFi connection status, signal strength (RSSI), IP address, MAC address, and uptime. It then displays each piece of information in a clear format. The connection status is shown in large text (size 3) for emphasis, while other details use smaller text. If WiFi is not connected, some values show "N/A" or default values.

## Key Functions

- `drawStatusScreen()`: Main function that renders the entire status screen
- `wifiManagerIsConnected()`: Checks if WiFi is currently connected
- `WiFi.RSSI()`: Gets the WiFi signal strength (Received Signal Strength Indicator)
- `WiFi.localIP()`: Gets the device's IP address on the network
- `WiFi.macAddress()`: Gets the device's MAC address
- `millis()`: Gets the uptime in milliseconds (converted to days/hours/minutes/seconds)
- `renderHeader()`: Draws the screen header with title and page indicator
- `clearContentArea()`: Clears the content area below the header

## Code Structure

The screen follows a simple vertical layout:
1. Draw header with "System" title and page indicator (3)
2. Clear content area
3. Display "Network" label
4. Display connection status in large text ("Connected" or "Offline")
5. Display signal strength (RSSI) in dBm
6. Display IP address
7. Display MAC address
8. Display uptime in human-readable format (days, hours, minutes, seconds)

## Uptime Calculation

The uptime is calculated from `millis()` (milliseconds since startup) and converted to a human-readable format:
- Days: `uptimeMs / 86400000` (86400000 ms = 1 day)
- Hours: `(uptimeMs % 86400000) / 3600000` (remaining hours)
- Minutes: `(uptimeMs % 3600000) / 60000` (remaining minutes)
- Seconds: `(uptimeMs % 60000) / 1000` (remaining seconds)

The format displayed is: "Xd Xh Xm Xs" (e.g., "2d 5h 30m 15s")

## Signal Strength (RSSI)

RSSI (Received Signal Strength Indicator) is measured in dBm (decibels relative to milliwatt):
- Typical range: -30 (excellent) to -90 (poor)
- Negative numbers are normal - closer to 0 is better
- Only available when WiFi is connected

## WiFi Status Handling

If WiFi is not connected:
- Connection status shows "Offline"
- Signal strength shows "N/A"
- IP address shows "0.0.0.0"
- MAC address is always available (hardware identifier)

