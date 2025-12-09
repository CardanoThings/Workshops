# Blink Code

Basic LED blink example - your first Arduino sketch

## Overview

This is the classic "Hello World" of embedded programming. The blink sketch makes an LED connected to your microcontroller turn on and off repeatedly, creating a blinking pattern. It's the perfect first program to learn the basics of Arduino programming.

## Requirements

- ESP32 or ESP8266 based microcontroller
- Arduino IDE
- LED with current-limiting resistor (220Ω-1kΩ) - optional if your board has a built-in LED

## How It Works

### Code Structure

The Arduino programming model uses two main functions:

- **`setup()`**: Called once when the microcontroller starts up. Used for initialization.
- **`loop()`**: Called repeatedly, forever, as long as the device is powered on. This is where your main program logic runs.

### Understanding the Code

1. **`#define LED_PIN 8`**
   - Creates a constant that maps pin 8 to `LED_PIN`
   - This makes the code more readable and easier to modify
   - If you need to change the pin, you only change it in one place
   - The `#define` directive is a preprocessor macro - it replaces all instances of `LED_PIN` with `8` before compilation

2. **`setup()` Function**
   - Called once when the microcontroller starts up
   - `pinMode(LED_PIN, OUTPUT)` sets pin 8 as an output pin
   - OUTPUT mode means the pin can supply voltage (HIGH = 3.3V for ESP32) or ground (LOW = 0V)
   - This is necessary before you can control the LED

3. **`loop()` Function**
   - Called repeatedly, over and over again
   - This is where the blinking happens:
     - `digitalWrite(LED_PIN, HIGH)` - Turns the LED on by setting the pin to HIGH (3.3V)
     - `delay(1000)` - Pauses the program for 1000 milliseconds (1 second)
     - `digitalWrite(LED_PIN, LOW)` - Turns the LED off by setting the pin to LOW (0V)
     - `delay(1000)` - Pauses again for 1 second
   - The cycle repeats forever, creating the blinking effect

### Pin Configuration

- **Pin 8** is used to control an external LED
- You'll need to connect an LED with a current-limiting resistor (220Ω-1kΩ) to this pin
- **Note**: If you're using the Cheap Yellow Display (CYD), the LED is connected to pin 4 instead

### Hardware Connection

To connect an external LED:

1. Connect the positive leg (anode, longer leg) of the LED to pin 8 through a 220Ω-1kΩ resistor
2. Connect the negative leg (cathode, shorter leg) of the LED to GND (ground)
3. The resistor limits the current to protect both the LED and the microcontroller

### Timing

- Each cycle (on + off) takes 2 seconds total
- LED is ON for 1 second
- LED is OFF for 1 second
- You can modify the `delay()` values to change the blink speed

## Installation

1. Open Arduino IDE
2. Copy the code from `blink-code.ino`
3. Paste it into a new sketch
4. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
5. Select the correct port: **Tools → Port → [Your Port]**
6. Click the Upload button (→ arrow icon)
7. Watch the LED blink!

## Usage

After uploading, the LED should start blinking immediately. The pattern will continue as long as the microcontroller is powered on.

## Troubleshooting

- **LED doesn't blink**: 
  - Check your wiring connections
  - Verify the pin number matches your hardware (pin 8 for ESP32-C3, pin 4 for CYD)
  - Make sure the resistor is connected properly
  - Try a different LED

- **Wrong blink speed**: 
  - Adjust the `delay()` values in the `loop()` function
  - Smaller values = faster blinking
  - Larger values = slower blinking

- **Upload errors**: 
  - Make sure you've selected the correct board and port
  - Check that your USB cable supports data transfer (not just charging)
  - Try pressing the BOOT button on your ESP32 during upload

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/01-basics/arduino-setup)

## Next Steps

Once you've mastered the blink sketch, try:
- Changing the blink speed
- Making the LED blink in patterns (Morse code, SOS signal, etc.)
- Controlling multiple LEDs
- Using the built-in LED (if your board has one)
