# Simple AHT10 Reading Code

Simple AHT10 sensor reading example - test wiring and verify sensor functionality

## Overview

This simple example demonstrates basic AHT10 sensor reading without any display hardware. It's perfect for testing your wiring and verifying that your sensor is working correctly before adding display functionality.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **AHT10 Temperature and Humidity Sensor** module
- Arduino IDE with ESP32 board support installed
- **Required Libraries**:
  - Wire (built-in for ESP32)
  - Adafruit AHT10 (must be installed via Library Manager)
  - Adafruit BusIO (dependency, installed automatically)
  - Adafruit Unified Sensor (dependency, installed automatically)
- USB cable for programming and power
- Breadboard and jumper wires (optional, for prototyping)

## How It Works

### Key Concepts

- **I2C Communication**: The AHT10 uses standard I2C pins on ESP32 (GPIO 8 for SDA and GPIO 9 for SCL). The library handles I2C initialization automatically.
- **Sensor Library**: The Adafruit AHT10 library provides easy-to-use functions like `getEvent()` that handle the complex I2C communication protocol and return both temperature and humidity in a single call.
- **Error Detection**: The sensor library's `begin()` function returns false if the sensor is not found, allowing you to detect initialization errors.
- **Serial Output**: All sensor readings are printed to the serial monitor, making it easy to debug and verify sensor functionality.

### Reading Sequence

1. **Initialization (`setup()`)**:
   - Initializes serial communication for debugging output (115200 baud)
   - Initializes the AHT10 sensor with `aht.begin()`, which returns true if successful
   - If initialization fails, the code halts with an error message

2. **Main Loop (`loop()`)**:
   - Creates `sensors_event_t` structures to hold sensor readings
   - Both temperature and humidity are read simultaneously using `aht.getEvent(&humidity, &temp)`
   - Temperature is accessed via `temp.temperature` and humidity via `humidity.relative_humidity`
   - Readings are printed to the serial monitor
   - The code waits 500ms before taking the next reading
   - The process repeats continuously, providing regular sensor updates

### Code Structure

#### Library and Sensor Object

```cpp
#include <Adafruit_AHT10.h>

Adafruit_AHT10 aht;
```

- **`#include <Adafruit_AHT10.h>`**: Includes the Adafruit AHT10 library
- **`Adafruit_AHT10 aht`**: Creates a sensor object named `aht` for accessing sensor functions

#### Setup Function

1. **Serial Communication**:
   ```cpp
   Serial.begin(115200);
   ```
   - Initializes serial communication at 115200 baud
   - **Note**: Set Serial Monitor to 115200 baud to view messages

2. **Sensor Initialization**:
   ```cpp
   if (!aht.begin()) {
       Serial.println("Could not find AHT10? Check wiring");
       while (1) delay(10);
   }
   ```
   - Attempts to initialize the AHT10 sensor
   - **`begin()`**: Returns `true` if sensor is found, `false` if not found
   - If initialization fails, prints error message and halts execution
   - This prevents the code from running with invalid sensor readings

#### Loop Function

1. **Sensor Event Structures**:
   ```cpp
   sensors_event_t humidity, temp;
   ```
   - Creates structures to hold sensor readings
   - `humidity`: Stores humidity data
   - `temp`: Stores temperature data

2. **Reading Sensor Data**:
   ```cpp
   aht.getEvent(&humidity, &temp);
   ```
   - **`getEvent()`**: Reads both temperature and humidity simultaneously
   - Populates the `humidity` and `temp` structures with fresh sensor data
   - More efficient than reading temperature and humidity separately

3. **Accessing Readings**:
   ```cpp
   temp.temperature              // Temperature in Celsius
   humidity.relative_humidity    // Humidity as percentage (0-100)
   ```
   - Temperature is accessed via `temp.temperature` (in degrees Celsius)
   - Humidity is accessed via `humidity.relative_humidity` (as percentage)

4. **Serial Output**:
   ```cpp
   Serial.print("Temperature: ");
   Serial.print(temp.temperature);
   Serial.println(" degrees C");
   ```
   - Prints formatted readings to serial monitor
   - Makes it easy to verify sensor is working correctly

5. **Delay Between Readings**:
   ```cpp
   delay(500);  // 500 milliseconds
   ```
   - Waits 500ms before next reading
   - Prevents excessive sensor queries
   - Adjustable based on your needs

## Installation

### Step 1: Install Required Libraries

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. Install the following libraries:
   - **Adafruit AHT10** by Adafruit
   - **Adafruit BusIO** (dependency, usually installed automatically)
   - **Adafruit Unified Sensor** (dependency, usually installed automatically)

### Step 2: Wire Your AHT10 Sensor

Connect your AHT10 sensor to the ESP32-C3:

1. **VCC** → Connect to **3.3V** on ESP32-C3
   - AHT10 operates at 3.3V (not 5V!)

2. **GND** → Connect to **GND** on ESP32-C3

3. **SDA** → Connect to **GPIO 8** (standard SDA pin on ESP32)
   - SDA = Serial Data Line

4. **SCL** → Connect to **GPIO 9** (standard SCL pin on ESP32)
   - SCL = Serial Clock Line

**Note**: Most AHT10 modules include pull-up resistors on the I2C lines, so no additional components are needed.

### Step 3: Upload Code

1. Copy the code from `simple-sht-code.ino`
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
   Temperature: 23.45 degrees C
   Humidity: 55.67% rH
   Temperature: 23.46 degrees C
   Humidity: 55.68% rH
   ...
   ```

2. **Reading Frequency**:
   - Readings update every 500ms (0.5 seconds)
   - Adjust the `delay(500)` value to change update frequency

## I2C Wiring Details

### Pin Connections

The AHT10 sensor requires four connections:

- **VCC**: Connect to 3.3V power supply
- **GND**: Connect to ground
- **SDA**: Connect to GPIO 8 (standard SDA pin on ESP32)
- **SCL**: Connect to GPIO 9 (standard SCL pin on ESP32)

### I2C Address

- **AHT10 I2C Address**: `0x38` (fixed address)
- If you have multiple I2C devices, make sure they don't conflict
- Use the [I2C Scanner block](/blocks/esp32-i2c-scanner) to scan for connected devices

### Pull-up Resistors

- Most AHT10 modules include built-in pull-up resistors (usually 4.7kΩ)
- If your module doesn't have pull-ups, add 4.7kΩ resistors between:
  - SDA and 3.3V
  - SCL and 3.3V

## Troubleshooting

### Sensor Not Found Error

**Error**: `"Could not find AHT10? Check wiring"`

**Solutions**:
- **Check wiring**: Verify all four connections (VCC, GND, SDA, SCL)
- **Check power**: Ensure sensor is receiving 3.3V (not 5V!)
- **Check I2C pins**: Verify SDA is on GPIO 8 and SCL is on GPIO 9
- **Check pull-up resistors**: Ensure pull-up resistors are present (most modules have them built-in)
- **Check I2C address**: Verify sensor address is 0x38 (use I2C scanner)

### No Serial Output

- **Check Serial Monitor**: Ensure Serial Monitor is open and set to 115200 baud
- **Check USB cable**: Ensure cable supports data transfer (not just charging)
- **Check port**: Verify correct COM port is selected in Arduino IDE

### Incorrect Readings

- **Check power supply**: Ensure stable 3.3V power supply
- **Check sensor**: Verify sensor is not damaged or overheated
- **Check environment**: Ensure sensor is in appropriate environment (not exposed to extreme conditions)

### Library Errors

- **Reinstall library**: Uninstall and reinstall Adafruit AHT10 library
- **Check dependencies**: Ensure Adafruit BusIO and Adafruit Unified Sensor are installed
- **Check library version**: Use the latest version from Library Manager

## Important Notes

- **I2C Address**: The AHT10 has a fixed I2C address of 0x38. If you have multiple I2C devices, make sure they don't conflict.
- **Power Supply**: The AHT10 operates at 3.3V. Make sure your power supply is stable and provides adequate current.
- **Reading Frequency**: The example code reads every 500ms, but you can adjust this delay to suit your needs. More frequent readings provide faster updates but consume more power.
- **Standard I2C Pins**: On ESP32, GPIO 8 and GPIO 9 are the standard SDA and SCL pins respectively. The library uses these by default, so no pin configuration is needed.

## Customization

### Change Reading Frequency

To read more or less frequently:

```cpp
delay(1000);  // Read every 1 second (slower)
delay(250);   // Read every 250ms (faster)
```

### Add Calculations

Calculate dew point or heat index:

```cpp
// Dew point calculation (simplified)
float dewPoint = temp.temperature - ((100 - humidity.relative_humidity) / 5);

// Heat index calculation (simplified, for temperatures above 80°F)
// Note: Requires conversion to Fahrenheit first
```

### Add Timestamps

Add timestamps to readings:

```cpp
Serial.print("Time: ");
Serial.print(millis() / 1000);
Serial.print("s - ");
Serial.print("Temperature: ");
Serial.println(temp.temperature);
```

### Store Readings

Store readings in an array to track trends:

```cpp
float tempHistory[10];
int historyIndex = 0;

// In loop():
tempHistory[historyIndex] = temp.temperature;
historyIndex = (historyIndex + 1) % 10;
```

### Add Conditional Logic

Trigger actions based on sensor values:

```cpp
if (temp.temperature > 25.0) {
    Serial.println("Temperature is high!");
    // Add your action here (e.g., turn on fan)
}

if (humidity.relative_humidity > 70.0) {
    Serial.println("Humidity is high!");
    // Add your action here
}
```

## Next Steps

Once you've verified your sensor is working, try:

- **Add Display**: Display readings on an OLED or TFT screen
- **Add WiFi**: Send sensor data to a cloud service or API
- **Add Logging**: Store readings to SD card or EEPROM
- **Add Alerts**: Trigger notifications when values exceed thresholds
- **Add Calculations**: Calculate dew point, heat index, or other derived values

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/03-input-and-write/connect-and-read-sensor-data)

## Additional Resources

- [Adafruit AHT10 Library](https://github.com/adafruit/Adafruit_AHTX0) - Official library repository
- [AHT10 Datasheet](https://www.adafruit.com/product/4566) - Sensor specifications and technical details
- [I2C Communication Guide](https://learn.adafruit.com/i2c-addresses) - Understanding I2C addresses
- [ESP32 I2C Scanner Block](/blocks/esp32-i2c-scanner) - Scan for I2C devices on your bus

## Notes

- This is a simple test example - perfect for verifying setup
- No display hardware required - all output goes to Serial Monitor
- AHT10 provides factory-calibrated readings with high accuracy
- I2C communication is handled automatically by the library
- Sensor readings update every 500ms by default
