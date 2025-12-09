# AHT10 with OLED Display Code

Read temperature and humidity from AHT10 sensor and display on 1.3" OLED screen (SH1106 controller)

## Overview

This project demonstrates how to read temperature and humidity data from an AHT10 sensor using I2C communication and display it on an OLED screen. The AHT10 is a high-precision digital sensor that provides accurate measurements with factory calibration, making it ideal for environmental monitoring projects.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **AHT10 Temperature and Humidity Sensor** module
- **1.3" OLED Display** (SH1106 controller) - I2C interface
- Arduino IDE with ESP32 board support installed
- **Required Libraries**:
  - Wire (built-in for ESP32)
  - Adafruit AHT10 (must be installed via Library Manager)
  - Adafruit BusIO (dependency, installed automatically)
  - Adafruit Unified Sensor (dependency, installed automatically)
  - Adafruit SH110X (must be installed via Library Manager)
  - Adafruit GFX Library (dependency, installed automatically)
- USB cable for programming and power
- Breadboard and jumper wires (optional, for prototyping)

## How It Works

### Key Concepts

- **I2C Communication**: The AHT10 uses the I2C (Inter-Integrated Circuit) protocol, which requires only two wires (SDA for data and SCL for clock) to communicate with the microcontroller. This makes it simpler than single-wire protocols like DHT sensors.
- **Sensor Initialization**: The AHT10 sensor is initialized through the I2C bus using the Wire library. The sensor has a default I2C address (0x38) and doesn't require additional configuration.
- **Non-blocking Timing**: Using `millis()` instead of `delay()` allows the microcontroller to update the display while checking for new sensor data, creating a responsive user interface.
- **Display Updates**: The OLED display is updated every 2 seconds with fresh sensor readings, providing real-time environmental data visualization.

### Program Flow

1. **Initialization (`setup()`)**:
   - Initializes serial communication for debugging (115200 baud)
   - Initializes the AHT10 sensor on the I2C bus
   - Initializes the OLED display on the same I2C bus
   - Shows startup messages on both serial monitor and display

2. **Main Loop (`loop()`)**:
   - Checks if 2 seconds have passed since last sensor read
   - Reads sensor data when interval is reached
   - Updates display with new readings
   - Repeats continuously without blocking

3. **Sensor Reading (`readSensorData()`)**:
   - Creates `sensors_event_t` structures to hold readings
   - Calls `aht.getEvent(&humidity_event, &temp_event)` to read both values simultaneously
   - Stores readings in global variables
   - Prints readings to serial monitor for debugging

4. **Display Update (`displayData()`)**:
   - Clears display buffer
   - Displays temperature with large text (size 3)
   - Displays humidity with smaller text (size 1)
   - Updates physical display

### Sensor Reading Process

1. The code initializes the AHT10 sensor on startup using standard I2C pins (GPIO 8/9)
2. The OLED display is initialized on the same I2C bus
3. Every 2 seconds, the code reads temperature and humidity from the sensor
4. The `readSensorData()` function calls `aht.getEvent(&humidity, &temp)` to get both temperature and humidity readings simultaneously
5. Sensor readings are stored in `sensors_event_t` structures, with temperature accessed via `temp.temperature` and humidity via `humidity.relative_humidity`
6. Valid readings are stored in global variables and printed to the serial monitor
7. The `displayData()` function updates the OLED screen with formatted sensor data
8. The display shows temperature and humidity values for easy reading
9. The process repeats continuously, providing real-time updates of environmental conditions

### I2C Communication

Both the AHT10 sensor and OLED display communicate via I2C protocol:

- **SDA (Serial Data Line)**: Carries data between devices and microcontroller. Connect both AHT10 and OLED to GPIO 8 (standard SDA pin on ESP32).
- **SCL (Serial Clock Line)**: Provides the clock signal for synchronization. Connect both AHT10 and OLED to GPIO 9 (standard SCL pin on ESP32).
- **I2C Addresses**: Each device has a unique address:
  - AHT10 sensor: 0x38 (fixed address)
  - OLED display: 0x3C or 0x3D (check your display's documentation)
  - The libraries handle addressing automatically. If you're not sure what addresses your devices use, use the [I2C Scanner block](/blocks/esp32-i2c-scanner) to scan for all connected I2C devices.
- **Multiple Devices on One Bus**: I2C supports multiple devices on the same bus because each device has a unique address. Both the sensor and display can share the same SDA and SCL lines.
- **Pull-up Resistors**: I2C requires pull-up resistors (usually 4.7kΩ) on both SDA and SCL lines. Most modules include these resistors.

### Display Functions

#### `readSensorData()`

Reads temperature and humidity from the AHT10 sensor via I2C using `getEvent(&humidity, &temp)`. Stores readings in global variables and prints data to serial monitor for debugging.

**Process**:
1. Creates `sensors_event_t` structures for humidity and temperature
2. Calls `aht.getEvent()` to read both values simultaneously
3. Extracts temperature from `temp_event.temperature`
4. Extracts humidity from `humidity_event.relative_humidity`
5. Stores values in global variables
6. Prints formatted output to serial monitor

#### `displayData()`

Updates the OLED display with current sensor readings. Clears the screen, displays a title, and shows temperature and humidity in different sizes for visual distinction.

**Process**:
1. Clears display buffer
2. Sets text size and color
3. Displays "Temperature" label
4. Displays temperature value in large text (size 3)
5. Displays "Humidity" label
6. Displays humidity value
7. Updates physical display with `display.display()`

#### Error Handling

The code checks for sensor initialization failures using `begin()` which returns false if the sensor is not found, halting execution to prevent invalid readings.

## Installation

### Step 1: Install Required Libraries

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. Install the following libraries:
   - **Adafruit AHT10** by Adafruit
   - **Adafruit SH110X** by Adafruit
   - **Adafruit BusIO** (dependency, usually installed automatically)
   - **Adafruit Unified Sensor** (dependency, usually installed automatically)
   - **Adafruit GFX Library** (dependency, usually installed automatically)

### Step 2: Wire Your Components

#### AHT10 Sensor Connections

1. **VCC** → Connect to **3.3V** on ESP32-C3
2. **GND** → Connect to **GND** on ESP32-C3
3. **SDA** → Connect to **GPIO 8** (standard SDA pin on ESP32)
4. **SCL** → Connect to **GPIO 9** (standard SCL pin on ESP32)

#### OLED Display Connections

1. **VCC** → Connect to **3.3V** on ESP32-C3 (can share with AHT10)
2. **GND** → Connect to **GND** on ESP32-C3 (can share with AHT10)
3. **SDA** → Connect to **GPIO 8** (same as AHT10 - shared I2C bus)
4. **SCL** → Connect to **GPIO 9** (same as AHT10 - shared I2C bus)

**Note**: Both devices share the same I2C bus (SDA and SCL lines) because they have different I2C addresses.

### Step 3: Configure the Code

**Before uploading**, check the OLED display address:

```cpp
#define SCREEN_ADDRESS 0x3C  // Change to 0x3D if your display uses that address
```

If you're not sure, use the [I2C Scanner block](/blocks/esp32-i2c-scanner) to find your display's address.

### Step 4: Upload Code

1. Copy the code from `sht-21-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor (115200 baud) to see sensor readings

## Usage

After uploading, you should see:

1. **Serial Monitor Output**:
   ```
   Adafruit AHT10 demo!
   AHT10 found
   OLED Display initialized!
   Temperature: 23.45 degrees C
   Humidity: 55.67% rH
   ...
   ```

2. **OLED Display**:
   - Shows "Initializing..." then "Ready!" on startup
   - Displays temperature in large text (size 3)
   - Displays humidity below temperature
   - Updates every 2 seconds

3. **Update Frequency**:
   - Sensor readings: Every 2 seconds
   - Display updates: Every 2 seconds (synchronized with sensor reads)

## Display Layout

The OLED display shows:

- **Temperature**: Large text (size 3) showing current temperature in Celsius
- **Humidity**: Smaller text (size 1) showing current humidity percentage

The display is 128x64 pixels, providing a clear, readable interface for monitoring environmental conditions.

## Troubleshooting

### Sensor Not Found Error

**Error**: `"Could not find AHT10? Check wiring"`

**Solutions**:
- **Check wiring**: Verify all four connections (VCC, GND, SDA, SCL)
- **Check power**: Ensure sensor is receiving 3.3V (not 5V!)
- **Check I2C pins**: Verify SDA is on GPIO 8 and SCL is on GPIO 9
- **Check pull-up resistors**: Ensure pull-up resistors are present
- **Use I2C scanner**: Use the [I2C Scanner block](/blocks/esp32-i2c-scanner) to verify sensor is detected at address 0x38

### Display Not Working

**Error**: `"SH1106 allocation failed"`

**Solutions**:
- **Check wiring**: Verify all four connections (VCC, GND, SDA, SCL)
- **Check address**: Verify `SCREEN_ADDRESS` matches your display (0x3C or 0x3D)
- **Use I2C scanner**: Scan for connected devices to find display address
- **Check power**: Ensure display is receiving 3.3V power
- **Check display type**: Verify you have SH1106 controller (not SSD1306)

### No Display Updates

- **Check sensor**: Verify sensor is reading correctly (check serial monitor)
- **Check timing**: Verify `readInterval` is set correctly (2000ms)
- **Check display function**: Ensure `display.display()` is being called

### Incorrect Readings

- **Check sensor**: Verify sensor is not damaged
- **Check environment**: Ensure sensor is in appropriate environment
- **Check power supply**: Ensure stable 3.3V power supply

## Customization

### Change Update Frequency

To update more or less frequently:

```cpp
const unsigned long readInterval = 1000;  // Every 1 second (faster)
const unsigned long readInterval = 5000;  // Every 5 seconds (slower)
```

### Change Display Layout

Modify the `displayData()` function to change how data is displayed:

```cpp
// Different text sizes
display.setTextSize(2);  // Medium text
display.setTextSize(4);  // Very large text

// Different colors (for color OLEDs)
display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // Inverted
```

### Add More Information

Display additional calculated values:

```cpp
// Add dew point calculation
float dewPoint = temperature - ((100 - humidity) / 5);
display.print("Dew Point: ");
display.println(dewPoint, 1);
```

### Add WiFi Connectivity

Send sensor data to a cloud service or API:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

// Add WiFi connection code
// Send data via HTTP POST
```

### Add Data Logging

Store readings to SD card or EEPROM:

```cpp
// Log readings to SD card
File dataFile = SD.open("sensor.txt", FILE_WRITE);
dataFile.println(String(temperature) + "," + String(humidity));
dataFile.close();
```

## Customization Ideas

- **Add data logging**: Store readings to SD card or send data to a cloud service
- **Create graphs**: Show temperature and humidity trends over time
- **Add alerts**: Trigger notifications when values exceed certain thresholds
- **Display calculated values**: Show dew point, heat index, or other derived values
- **Add WiFi connectivity**: Send sensor data to a remote server
- **Create multi-screen interface**: Add buttons to switch between different views

## Next Steps

Once your sensor and display are working, try:

- **Add WiFi**: Send sensor data to a cloud service or API
- **Add Logging**: Store readings to SD card or EEPROM
- **Add Alerts**: Trigger actions when values exceed thresholds
- **Add Calculations**: Calculate dew point, heat index, or other derived values
- **Create Dashboards**: Build web interfaces to view sensor data remotely

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/03-input-and-write/connect-and-read-sensor-data)

## Additional Resources

- [Adafruit AHT10 Library](https://github.com/adafruit/Adafruit_AHTX0) - Official library repository
- [Adafruit SH110X Library](https://github.com/adafruit/Adafruit_SH110x) - OLED display library
- [AHT10 Datasheet](https://www.adafruit.com/product/4566) - Sensor specifications
- [I2C Communication Guide](https://learn.adafruit.com/i2c-addresses) - Understanding I2C addresses
- [ESP32 I2C Scanner Block](/blocks/esp32-i2c-scanner) - Scan for I2C devices on your bus

## Notes

- Both sensor and display share the same I2C bus (SDA/SCL lines)
- I2C addresses must be unique: AHT10 (0x38), OLED (0x3C or 0x3D)
- Display updates every 2 seconds, synchronized with sensor reads
- Non-blocking timing allows responsive updates without delays
- Sensor readings are printed to serial monitor for debugging
