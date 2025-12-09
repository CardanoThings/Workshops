# Arduino Code - Post Sensor Data to API

ESP32 microcontroller code that reads temperature and humidity from an AHT10 sensor and sends the data to your Node.js API server over WiFi.

## Overview

This code reads temperature and humidity from your sensor and sends it to your computer's API server. Think of it like sending a text message - your microcontroller reads the sensor, packages the data, and sends it to your server over WiFi.

## Requirements

### Hardware

- **ESP32 microcontroller** (ESP32-C3, ESP32-S3, or similar)
- **AHT10 temperature and humidity sensor**
- **Breadboard and jumper wires** (for connections)
- **USB cable** (for programming and power)

### Software

- **Arduino IDE** (v1.8.19 or higher) or **PlatformIO**
- **ESP32 Board Support** installed in Arduino IDE
- **Required Libraries**:
  - `WiFi` (built-in ESP32 library)
  - `HTTPClient` (built-in ESP32 library)
  - `ArduinoJson` (by Benoit Blanchon) - Install via Library Manager
  - `Adafruit AHT10` (by Adafruit) - Install via Library Manager
  - `Adafruit BusIO` (dependency) - Install via Library Manager
  - `Adafruit Unified Sensor` (dependency) - Install via Library Manager

## Installation

### Step 1: Install Arduino IDE

1. Download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
2. Install the IDE on your computer

### Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. In **Additional Board Manager URLs**, add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "ESP32" and install **esp32 by Espressif Systems**

### Step 3: Install Required Libraries

1. Go to **Sketch** → **Include Library** → **Manage Libraries**
2. Search for and install:
   - **ArduinoJson** by Benoit Blanchon
   - **Adafruit AHT10** by Adafruit
   - **Adafruit BusIO** by Adafruit (dependency)
   - **Adafruit Unified Sensor** by Adafruit (dependency)

### Step 4: Select Your Board

1. Go to **Tools** → **Board** → **ESP32 Arduino**
2. Select your specific ESP32 board (e.g., "ESP32C3 Dev Module" or "ESP32S3 Dev Module")

### Step 5: Select Port

1. Connect your ESP32 to your computer via USB
2. Go to **Tools** → **Port**
3. Select the COM port (Windows) or `/dev/ttyUSB*` (Linux) or `/dev/cu.*` (macOS)

## Wiring

### AHT10 Sensor Connections

Connect the AHT10 sensor to your ESP32:

| AHT10 Pin | ESP32 Pin | Description |
|-----------|-----------|-------------|
| VCC       | 3.3V      | Power (3.3V) |
| GND       | GND       | Ground |
| SDA       | GPIO 8    | I2C Data Line |
| SCL       | GPIO 9    | I2C Clock Line |

**Note**: For ESP32-C3, GPIO 8 and GPIO 9 are the default I2C pins. For other ESP32 variants, check your board's pinout.

## Configuration

Before uploading the code, you need to update these values:

### WiFi Credentials

```cpp
const char* ssid = "Your SSID";        // Replace with your WiFi network name
const char* password = "Your Password"; // Replace with your WiFi password
```

### API Server URL

```cpp
const char* apiUrl = "http://YOUR_SERVER_IP:3000/data";
```

Replace `YOUR_SERVER_IP` with your computer's IP address where your Node.js API server is running.

**To find your IP address:**

- **Windows**: Open Command Prompt and run `ipconfig` - look for "IPv4 Address"
- **macOS/Linux**: Open Terminal and run `ifconfig` or `ip addr` - look for your network interface's IP address

### Timing Configuration

```cpp
const unsigned long readingInterval = 300000;  // Read every 5 minutes (300000 milliseconds)
```

You can change this to send data more or less frequently:
- `60000` = 1 minute
- `300000` = 5 minutes (default)
- `600000` = 10 minutes

### Send Once Flag

```cpp
const bool sendOnce = true;  // If true, send sensor data only once
```

- **`true`**: Sensor data will only be sent once, even if the code runs for a long time. This prevents creating too many blockchain transactions while testing.
- **`false`**: Sensor data will be sent every `readingInterval` continuously, creating a new NFT each time (if your API server mints NFTs).

## How the Code Works

### Setup Phase

1. **Initialize Serial Communication**: Sets up serial monitor for debugging (115200 baud rate)
2. **Initialize AHT10 Sensor**: Checks if the sensor is connected and working
3. **Connect to WiFi**: Connects to your WiFi network using the provided credentials
4. **Print IP Address**: Displays the ESP32's IP address on the serial monitor

### Main Loop

1. **Check WiFi Connection**: Verifies WiFi is still connected, reconnects if needed
2. **Check Timing**: Checks if enough time has passed since the last reading
3. **Check Send Once Flag**: If `sendOnce` is `true` and data has already been sent, skips sending
4. **Send Sensor Data**: If allowed, reads sensor and sends data to API server
5. **Update Timestamp**: Updates the last reading timestamp

### Sensor Reading Process

1. **Read Sensor**: Uses `aht.getEvent()` to read both temperature and humidity
2. **Extract Values**: Gets temperature (Celsius) and humidity (percentage)
3. **Print to Serial**: Displays readings on serial monitor for debugging
4. **Create JSON**: Packages data into JSON format
5. **Send HTTP POST**: Sends data to your API server
6. **Handle Response**: Prints server response or error messages

## Code Structure

### Libraries

```cpp
#include <WiFi.h>              // WiFi connectivity
#include <HTTPClient.h>       // HTTP client for API calls
#include <ArduinoJson.h>      // JSON parsing and creation
#include <Adafruit_AHT10.h>   // Adafruit AHT10 library
```

### Global Variables

- **`aht`**: AHT10 sensor object
- **`ssid`**: WiFi network name
- **`password`**: WiFi password
- **`apiUrl`**: API server URL
- **`lastReading`**: Timestamp of last sensor reading
- **`readingInterval`**: Time between readings (milliseconds)
- **`sendOnce`**: Flag to send data only once
- **`dataSent`**: Tracks if data has been sent

### Functions

#### `setup()`

Initializes:
- Serial communication
- AHT10 sensor
- WiFi connection

#### `loop()`

Main program loop that:
- Maintains WiFi connection
- Checks timing intervals
- Calls `sendSensorData()` when needed

#### `sendSensorData()`

1. Reads temperature and humidity from sensor
2. Creates JSON payload with sensor data
3. Sends HTTP POST request to API server
4. Handles response and errors

## Usage

### Step 1: Update Configuration

1. Open `post-sensor-data.ino` in Arduino IDE
2. Update WiFi credentials (`ssid` and `password`)
3. Update API server URL (`apiUrl`) with your computer's IP address
4. Adjust `readingInterval` if needed
5. Set `sendOnce` to `false` if you want continuous sending

### Step 2: Upload Code

1. Connect your ESP32 to your computer
2. Select the correct board and port in Arduino IDE
3. Click **Upload** button (or press `Ctrl+U` / `Cmd+U`)
4. Wait for upload to complete

### Step 3: Open Serial Monitor

1. Go to **Tools** → **Serial Monitor**
2. Set baud rate to **115200**
3. You should see:
   - "Temperature Sensor NFT Demo!"
   - "AHT10 found"
   - "Connected to WiFi"
   - Your ESP32's IP address
   - Sensor readings and API responses

### Step 4: Verify Data Sending

Watch the serial monitor for:
- Temperature and humidity readings
- "Sending data to API..." messages
- HTTP response codes (200 = success)
- Server response messages

## Troubleshooting

### Sensor Not Found

**Error**: "Could not find AHT10? Check wiring"

**Solutions**:
- Check wiring connections (VCC, GND, SDA, SCL)
- Verify sensor is powered (3.3V, not 5V)
- Check I2C pins match your ESP32 board
- Try different I2C pins if default ones don't work
- Ensure pull-up resistors are present (some boards have them built-in)

### WiFi Connection Failed

**Error**: "Connecting to WiFi..." (never connects)

**Solutions**:
- Verify WiFi SSID and password are correct
- Check WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Move ESP32 closer to WiFi router
- Check router allows new device connections
- Try restarting your router

### API Request Failed

**Error**: "Error in HTTP request" or negative HTTP response code

**Solutions**:
- Verify API server is running on your computer
- Check IP address is correct in `apiUrl`
- Ensure firewall allows connections on port 3000
- Verify API server URL endpoint is `/data`
- Check API server logs for errors
- Test API server with a browser or Postman first

### JSON Parsing Errors

**Error**: JSON-related errors in serial monitor

**Solutions**:
- Ensure ArduinoJson library is installed
- Check JSON payload format matches API expectations
- Verify `DynamicJsonDocument` size (512 bytes) is sufficient

### Data Not Sending

**Issue**: No "Sending data to API..." messages

**Solutions**:
- Check `sendOnce` flag - if `true` and `dataSent` is `true`, it won't send again
- Verify `readingInterval` timing - wait for the interval to pass
- Check serial monitor for any error messages
- Restart ESP32 to reset `dataSent` flag

## Testing Tips

### Test WiFi Connection First

Before testing the full code, verify WiFi works:

```cpp
void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected!");
    Serial.println(WiFi.localIP());
}
```

### Test Sensor Reading

Test sensor separately:

```cpp
void loop() {
    sensors_event_t humidity_event, temp_event;
    aht.getEvent(&humidity_event, &temp_event);
    Serial.print("Temp: ");
    Serial.print(temp_event.temperature);
    Serial.print(" Humidity: ");
    Serial.println(humidity_event.relative_humidity);
    delay(2000);
}
```

### Test API Connection

Test API server with a simple HTTP request:

```cpp
HTTPClient http;
http.begin("http://YOUR_SERVER_IP:3000/health");
int code = http.GET();
Serial.println(code);
http.end();
```

## Important Notes

### The sendOnce Flag

The code includes a `sendOnce` flag that's set to `true` by default. This is a safety feature for testing:

- **When `sendOnce` is `true`**: The sensor data will only be sent once, even if the code runs for a long time. This prevents creating too many blockchain transactions while you're testing and debugging.
- **When `sendOnce` is `false`**: The sensor data will be sent every 5 minutes continuously, creating a new NFT each time (if your API server mints NFTs).

This is especially important when your API server automatically mints NFTs - you don't want to accidentally create hundreds of NFTs while testing!

### Network Requirements

- ESP32 and your computer must be on the **same WiFi network**
- Your computer's firewall must allow incoming connections on port 3000
- Your API server must be running and accessible

### Power Considerations

- ESP32 can be powered via USB or external power supply
- For battery-powered projects, consider using deep sleep mode between readings
- WiFi consumes significant power - reduce `readingInterval` for battery projects

## Next Steps

- **Add deep sleep mode**: Save power by putting ESP32 to sleep between readings
- **Add error retry logic**: Retry failed API requests automatically
- **Add multiple sensors**: Read from multiple sensors and send combined data
- **Add local storage**: Store readings in flash memory if API is unavailable
- **Add OTA updates**: Update code wirelessly without USB connection
- **Add status LED**: Visual feedback for WiFi connection and data sending

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/03-input-and-write/mint-sensor-data-on-chain)

## Additional Resources

- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/) - Official ESP32 Arduino documentation
- [Adafruit AHT10 Library](https://github.com/adafruit/Adafruit_AHTX0) - AHT10 sensor library documentation
- [ArduinoJson Documentation](https://arduinojson.org/) - JSON library documentation
- [ESP32 WiFi Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html) - WiFi library reference
- [HTTPClient Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/httpclient.html) - HTTP client library reference

## Notes

- Default reading interval is 5 minutes (300000 milliseconds)
- `sendOnce` flag prevents multiple sends during testing
- Sensor readings include temperature (Celsius) and humidity (percentage)
- Timestamp is generated using `millis()` (milliseconds since boot)
- Code includes WiFi reconnection logic for reliability
- Serial monitor output helps with debugging

