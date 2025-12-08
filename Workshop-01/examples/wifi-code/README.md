# WiFi Connection Code

WiFi connection example for ESP32/ESP8266 - Connect your microcontroller to the Internet

## Overview

This sketch demonstrates how to connect an ESP32 microcontroller to a WiFi network. Once connected, your device can access the Internet, make API calls, and communicate with other devices on your network. This is a fundamental step for IoT projects that require network connectivity.

## Requirements

- ESP32 or ESP8266 based microcontroller
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- USB cable for programming and power

## How It Works

### Code Structure

Like all Arduino sketches, this code uses two main functions:
- **`setup()`**: Called once at startup - initializes WiFi and establishes connection
- **`loop()`**: Called repeatedly - monitors connection and handles reconnection

### Understanding the Code

#### Library and Configuration

1. **`#include <WiFi.h>`**
   - Includes the WiFi library for ESP32
   - This library provides all the functions needed to connect to WiFi networks
   - It's part of the ESP32 Arduino core, so no additional installation is needed

2. **WiFi Credentials**
   ```cpp
   const char* ssid = "Your SSID";      // Your WiFi network name
   const char* password = "Your Password"; // Your WiFi password
   ```
   - **SSID**: Service Set Identifier - the name of your WiFi network
   - **Password**: The network password (WPA2/WPA3)
   - **Important**: Replace these with your actual WiFi credentials before uploading!

#### Setup Function

3. **`Serial.begin(115200)`**
   - Initializes serial communication at 115200 baud rate
   - Baud rate = speed of data transmission (bits per second)
   - This allows you to see debug messages in the Serial Monitor
   - **Note**: Make sure your Serial Monitor is set to 115200 baud to see the messages

4. **`WiFi.mode(WIFI_STA)`**
   - Sets WiFi mode to Station (client mode)
   - Station mode means the ESP32 connects to an existing WiFi network (like a laptop or phone)
   - Alternative modes: `WIFI_AP` (Access Point - creates its own network) or `WIFI_AP_STA` (both)

5. **`WiFi.setTxPower(WIFI_POWER_8_5dBm)`**
   - Sets WiFi transmit power to 8.5 dBm
   - **Workaround for ESP32-C3 Super Mini**: This specific board variant sometimes has WiFi connectivity issues, and reducing the transmit power helps resolve them
   - For other ESP32 boards, you can remove this line or adjust the power level
   - Power levels: `WIFI_POWER_19_5dBm` (max), `WIFI_POWER_11dBm`, `WIFI_POWER_8_5dBm`, etc.

6. **`WiFi.begin(ssid, password)`**
   - Starts the WiFi connection process
   - Passes your network credentials to the WiFi library
   - This is non-blocking - it starts the connection but doesn't wait for it to complete

7. **Connection Loop**
   ```cpp
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   ```
   - **`WiFi.status()`**: Checks the current WiFi connection status
   - **`WL_CONNECTED`**: Constant that means "successfully connected"
   - The `while` loop keeps checking until connection is established
   - Prints "Connecting to WiFi..." every second to show progress
   - **Note**: If your credentials are wrong, this will loop forever!

8. **Success Messages**
   ```cpp
   Serial.println("Connected to WiFi");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   ```
   - Prints confirmation when connected
   - **`WiFi.localIP()`**: Gets the IP address assigned to your ESP32 by the router
   - This IP address is how other devices on your network can communicate with the ESP32

#### Loop Function

9. **Connection Monitoring**
   ```cpp
   if (WiFi.status() != WL_CONNECTED) {
       // Reconnection logic
   }
   ```
   - Continuously checks if WiFi connection is still active
   - WiFi can drop due to signal issues, router problems, or network changes
   - This check happens every loop cycle (very frequently)

10. **Automatic Reconnection**
    ```cpp
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Reconnected!");
    ```
    - If connection is lost, automatically attempts to reconnect
    - **`WiFi.reconnect()`**: Attempts to reconnect using the same credentials
    - Shows progress with dots (`.`) while waiting
    - Prints confirmation when reconnected
    - This ensures your device stays connected even if the network temporarily drops

### Connection States

The `WiFi.status()` function can return several values:
- **`WL_CONNECTED`**: Successfully connected to WiFi
- **`WL_NO_SSID_AVAIL`**: Network not found (wrong SSID or out of range)
- **`WL_CONNECT_FAILED`**: Connection failed (usually wrong password)
- **`WL_CONNECTION_LOST`**: Connection was established but then lost
- **`WL_DISCONNECTED`**: Not connected

## Configuration

### Step 1: Update WiFi Credentials

**Before uploading**, you MUST update these lines with your actual WiFi credentials:

```cpp
const char* ssid = "Your SSID";        // Change to your WiFi name
const char* password = "Your Password"; // Change to your WiFi password
```

**Finding your WiFi credentials:**
- **SSID**: Look at the list of available networks on your phone/computer - that's the SSID
- **Password**: The password you use to connect to your WiFi network

### Step 2: Optional - Adjust for Your Board

- **ESP32-C3 Super Mini**: Keep `WiFi.setTxPower(WIFI_POWER_8_5dBm);` (already included)
- **Other ESP32 boards**: You can remove this line or adjust the power level if needed

## Installation

1. Open Arduino IDE
2. Copy the code from `wifi-code.ino`
3. Paste it into a new sketch
4. **Update WiFi credentials** (SSID and password)
5. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
6. Select the correct port: **Tools → Port → [Your Port]**
7. Click the Upload button (→ arrow icon)
8. Open Serial Monitor: **Tools → Serial Monitor** (or `Ctrl+Shift+M` / `Cmd+Shift+M`)
9. Set baud rate to **115200** in the Serial Monitor
10. Watch the connection process!

## Usage

After uploading:

1. **Open Serial Monitor** (Tools → Serial Monitor)
2. **Set baud rate to 115200** (bottom-right of Serial Monitor)
3. You should see:
   - "Connecting to WiFi..." (repeated until connected)
   - "Connected to WiFi"
   - "IP address: "
   - Your ESP32's IP address (e.g., 192.168.1.100)

4. The device will now:
   - Stay connected to WiFi
   - Automatically reconnect if connection is lost
   - Print status messages to Serial Monitor

## Troubleshooting

### "Connecting to WiFi..." Forever

- **Wrong SSID**: Double-check the network name (case-sensitive!)
- **Wrong password**: Verify your WiFi password
- **Out of range**: Move closer to your WiFi router
- **5GHz network**: Some ESP32 boards only support 2.4GHz WiFi - make sure you're connecting to a 2.4GHz network
- **Router blocking**: Some routers block new devices - check router settings

### Connection Drops Frequently

- **Weak signal**: Move ESP32 closer to router or use a WiFi extender
- **Power issues**: Ensure stable power supply (USB cable can cause issues)
- **Interference**: Other devices (microwaves, Bluetooth) can interfere with 2.4GHz WiFi

### Serial Monitor Shows Nothing

- **Wrong baud rate**: Must be set to 115200
- **Wrong port**: Select the correct COM port in Tools → Port
- **USB cable**: Make sure it's a data cable, not just charging

### ESP32-C3 Super Mini Issues

- The code already includes `WiFi.setTxPower(WIFI_POWER_8_5dBm);` which should help
- If still having issues, try:
  - Moving closer to router
  - Using a different WiFi network (2.4GHz)
  - Checking for firmware updates

## Serial Monitor

The Serial Monitor is essential for debugging WiFi connections:

- **Open**: Tools → Serial Monitor or `Ctrl+Shift+M` (Windows/Linux) / `Cmd+Shift+M` (Mac)
- **Baud Rate**: Must match code (115200 in this example)
- **What you'll see**:
  - Connection attempts
  - Success/failure messages
  - IP address when connected
  - Reconnection messages if connection drops

## Next Steps

Once your ESP32 is connected to WiFi, you can:

- **Make HTTP requests**: Fetch data from APIs (weather, prices, etc.)
- **Create a web server**: Host a simple web page on your ESP32
- **Send data to cloud services**: Upload sensor readings to databases
- **Control devices remotely**: Use MQTT or other protocols
- **OTA updates**: Update firmware over WiFi

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/01-basics/arduino-setup)

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit WiFi credentials to version control** (Git)
- **Use environment variables or separate config files** for production code
- **Consider using WiFi Manager libraries** that let users enter credentials via web interface
- **Use WPA2/WPA3 encryption** on your WiFi network (not WEP or open networks)

## Additional Resources

- [ESP32 WiFi Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html)
- [Arduino WiFi Library Reference](https://www.arduino.cc/reference/en/libraries/wifi/)
- [Troubleshooting ESP32 WiFi Issues](https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/)
