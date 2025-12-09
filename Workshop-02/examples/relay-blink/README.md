# Relay Blink Code

Simple relay blink example for testing relay wiring and basic relay control

## Overview

This is a basic relay control example that demonstrates how to turn a relay ON and OFF in a continuous cycle. It's perfect for testing your relay wiring before adding blockchain integration or other complex functionality. The relay will turn ON for 2 seconds, then OFF for 2 seconds, creating a continuous blinking pattern.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **Relay Module** (single-channel relay module recommended)
- Arduino IDE with ESP32 board support installed
- USB cable for programming and power
- **Optional**: LED with resistor for low-voltage testing before using relay

## How It Works

### Code Structure

The sketch is very simple:
- **`setup()`**: Initializes serial communication and configures the relay pin
- **`loop()`**: Continuously turns relay ON and OFF in a 2-second cycle

### Understanding the Code

#### Pin Configuration

1. **Relay Pin Definition**
   ```cpp
   const int relayPin = 4;  // Change this to match your wiring
   ```
   - Defines which GPIO pin controls the relay
   - **Important**: Change this to match your actual wiring
   - Common pins: GPIO 2, GPIO 4, GPIO 5 (check ESP32-C3 pinout)

#### Setup Function

2. **Serial Communication**
   ```cpp
   Serial.begin(115200);
   delay(1000);
   ```
   - Initializes serial communication at 115200 baud
   - Waits 1 second for serial port to initialize
   - **Note**: Set Serial Monitor to 115200 baud to view messages

3. **Pin Mode Configuration**
   ```cpp
   pinMode(relayPin, OUTPUT);
   ```
   - Configures the relay pin as an OUTPUT
   - OUTPUT mode allows the microcontroller to control the pin voltage
   - Required before using `digitalWrite()`

4. **Initial State**
   ```cpp
   digitalWrite(relayPin, HIGH);  // Start with relay OFF
   ```
   - Sets relay to OFF state initially
   - For **active LOW relays**: HIGH = OFF, LOW = ON
   - For **active HIGH relays**: LOW = OFF, HIGH = ON
   - Most relay modules are active LOW

#### Loop Function

5. **Turn Relay ON**
   ```cpp
   Serial.println("Relay ON");
   digitalWrite(relayPin, LOW);  // LOW activates active LOW relays
   delay(2000);                  // Keep relay ON for 2 seconds
   ```
   - Sets pin to LOW (for active LOW relays)
   - Activates the relay (you should hear a click)
   - Waits 2 seconds while relay is ON
   - Prints status to Serial Monitor

6. **Turn Relay OFF**
   ```cpp
   Serial.println("Relay OFF");
   digitalWrite(relayPin, HIGH); // HIGH deactivates active LOW relays
   delay(2000);                  // Keep relay OFF for 2 seconds
   ```
   - Sets pin to HIGH (for active LOW relays)
   - Deactivates the relay (you should hear another click)
   - Waits 2 seconds while relay is OFF
   - Prints status to Serial Monitor

7. **Continuous Cycle**
   - The loop repeats indefinitely
   - Creates a 2-second ON, 2-second OFF pattern
   - You should hear clicking every 2 seconds

### Active LOW vs Active HIGH

**Understanding Relay Logic:**

Most relay modules are **active LOW**, meaning:
- **LOW (0V)** = Relay ON (activated)
- **HIGH (3.3V/5V)** = Relay OFF (deactivated)

Some relay modules are **active HIGH**, meaning:
- **HIGH (3.3V/5V)** = Relay ON (activated)
- **LOW (0V)** = Relay OFF (deactivated)

**How to Tell Which Type You Have:**
- **Check the relay module documentation**
- **Try the code as-is** (written for active LOW)
- **If it doesn't work**, reverse HIGH and LOW in the code
- **Listen for the click** - you'll hear it when the relay activates

## Installation

### Step 1: Wire Your Relay Module

Connect your relay module to the ESP32-C3:

1. **VCC** → Connect to **3.3V** or **5V** (check your relay module specifications)
2. **GND** → Connect to **GND** (ground)
3. **IN** (or SIG) → Connect to **GPIO 4** (or any available GPIO pin)

**Important**: Note which GPIO pin you connected the IN pin to - you'll need to update the code!

### Step 2: Configure the Code

**Before uploading**, update the relay pin number:

```cpp
const int relayPin = 4;  // Change this to match your wiring
```

If you connected the relay to GPIO 2, change it to:
```cpp
const int relayPin = 2;
```

### Step 3: Upload Code

1. Copy the code from `relay-blink-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor: **Tools → Serial Monitor** (or `Ctrl+Shift+M` / `Cmd+Shift+M`)
7. Set baud rate to **115200** in the Serial Monitor
8. Listen for the clicking!

## Usage

After uploading, you should observe:

1. **Serial Monitor Output**:
   ```
   Relay Blink Example
   Relay will turn ON for 2 seconds, then OFF for 2 seconds
   You should hear a click when the relay activates
   Relay ON
   Relay OFF
   Relay ON
   Relay OFF
   ...
   ```

2. **Physical Indicators**:
   - **Clicking sound**: You should hear a click every 2 seconds
   - **Status LED**: If your relay module has a status LED, it should light up when relay is ON
   - **Connected device**: If you've connected a device, it should turn on/off with the relay

## Testing Your Setup

### What You Should See/Hear

When the code is running correctly:
- ✅ **Clicking sound** every 2 seconds (the relay switching)
- ✅ **Status LED** on relay module turns on and off
- ✅ **Serial Monitor** shows "Relay ON" and "Relay OFF" messages
- ✅ **Connected device** (if wired) turns on and off with the relay

### Testing Without High Voltage

**Before connecting high-voltage devices**, test with a low-voltage LED:

1. Connect an LED with resistor to the relay output terminals (COM and NO)
2. Use a low-voltage power source (3.3V or 5V)
3. Verify the LED turns on/off with the relay
4. Once confirmed, you can proceed to high-voltage devices (if experienced)

## Troubleshooting

### No Clicking Sound

- **Check wiring**: Verify VCC, GND, and IN connections
- **Check pin number**: Ensure `relayPin` matches your wiring
- **Try reversing logic**: Change LOW to HIGH and HIGH to LOW (some relays are active HIGH)
- **Check power**: Verify relay module is receiving power (check VCC connection)
- **Check relay module**: Some modules need 5V, others work with 3.3V

### Relay Doesn't Turn On

- **Wrong pin**: Verify GPIO pin number in code matches your wiring
- **Active HIGH relay**: Try reversing HIGH and LOW in the code
- **Power issue**: Check if relay module is receiving power (VCC)
- **Faulty module**: Try a different relay module if available

### Relay Stays On (or Off) All the Time

- **Wrong logic**: Try reversing HIGH and LOW
- **Wiring issue**: Check IN pin connection
- **Pin configuration**: Ensure pin is set as OUTPUT in `setup()`

### Serial Monitor Shows Nothing

- **Wrong baud rate**: Must be set to 115200
- **Wrong port**: Select the correct COM port in Tools → Port
- **USB cable**: Ensure it's a data cable, not just charging

### Relay Clicks But Device Doesn't Work

- **High-voltage wiring**: Check COM and NO terminal connections
- **Power source**: Verify device is receiving power
- **Relay rating**: Ensure relay can handle your device's current/voltage
- **Safety**: Double-check high-voltage connections (if applicable)

## Customization

### Change Blink Speed

To make the relay blink faster or slower:

```cpp
delay(1000);  // 1 second (faster)
delay(5000);  // 5 seconds (slower)
```

Change both delays to keep ON and OFF times equal.

### Different ON/OFF Times

To have different ON and OFF durations:

```cpp
digitalWrite(relayPin, LOW);   // ON
delay(3000);                   // ON for 3 seconds
digitalWrite(relayPin, HIGH);  // OFF
delay(1000);                   // OFF for 1 second
```

### Use Non-Blocking Timing

To avoid blocking the code (useful for more complex projects):

```cpp
unsigned long lastChange = 0;
const unsigned long interval = 2000;  // 2 seconds

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastChange >= interval) {
        // Toggle relay
        digitalWrite(relayPin, !digitalRead(relayPin));
        lastChange = currentMillis;
    }
}
```

### Multiple Relays

To control multiple relays:

```cpp
const int relayPin1 = 4;
const int relayPin2 = 5;

void setup() {
    pinMode(relayPin1, OUTPUT);
    pinMode(relayPin2, OUTPUT);
}

void loop() {
    digitalWrite(relayPin1, LOW);
    delay(2000);
    digitalWrite(relayPin1, HIGH);
    
    digitalWrite(relayPin2, LOW);
    delay(2000);
    digitalWrite(relayPin2, HIGH);
}
```

## Safety Notes

⚠️ **Important Safety Considerations:**

- **Start with low voltage**: Test with LEDs before using high-voltage devices
- **Understand relay ratings**: Ensure relay can handle your device's current/voltage
- **High voltage warning**: Working with 110V/220V can be dangerous
- **Proper wiring**: Double-check all connections before powering on
- **Isolation**: Relays provide isolation, but always be cautious with high voltage
- **Experience required**: Only proceed with high-voltage devices if you have experience

## Next Steps

Once you've verified your relay works, try:

- **Blockchain integration**: Control relay based on wallet balance changes
- **Sensor triggers**: Activate relay based on sensor readings
- **Timed sequences**: Create complex on/off patterns
- **Multiple relays**: Control multiple devices independently
- **Web interface**: Control relay remotely via web server

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/light-up-the-tree)

## Additional Resources

- [Arduino digitalWrite() Reference](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/) - Learn about controlling digital pins
- [ESP32-C3 Pinout Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html) - GPIO pin reference
- [Relays Explained](https://www.youtube.com/watch?v=jXcdH1PgmMI) - Video tutorial explaining how relays work
- [Electrical Safety Guide](https://www.osha.gov/electrical) - Important safety information
