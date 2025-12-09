# Simple LED Blink Code

Simple WS2812 LED ring blink example for testing wiring and verifying LED ring functionality

## Overview

This is a basic WS2812 LED ring control example that demonstrates how to control addressable RGB LEDs. It lights up each of the 12 LEDs one at a time in white, creating a sequential blink pattern around the ring. This example is perfect for testing your wiring and verifying that your LED ring is working correctly before adding blockchain integration or other complex functionality.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **WS2812 LED Ring** (12 LEDs recommended)
- Arduino IDE with ESP32 board support installed
- **Adafruit NeoPixel library** (must be installed via Library Manager)
- USB cable for programming and power
- **Optional**: External 5V power supply for brighter displays

## How It Works

### Code Structure

The sketch is very simple:
- **`setup()`**: Initializes the NeoPixel library and sets brightness
- **`loop()`**: Cycles through all LEDs one at a time

### Understanding the Code

#### Library and Configuration

1. **`#include <Adafruit_NeoPixel.h>`**
   - Adafruit NeoPixel library for controlling WS2812 LEDs
   - **Must be installed** via Library Manager
   - Handles the complex timing protocol required by WS2812 LEDs

2. **Pin Definition**
   ```cpp
   #define LED_PIN 4
   ```
   - GPIO pin connected to LED ring IN (data input) pin
   - **Important**: Change this to match your wiring
   - Common pins: GPIO 2, GPIO 4, GPIO 5

3. **LED Count**
   ```cpp
   #define NUM_LEDS 12
   ```
   - Number of LEDs in your ring
   - This example uses 12 LEDs (standard ring size)
   - Adjust if your ring has a different number

4. **NeoPixel Object**
   ```cpp
   Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
   ```
   - Creates NeoPixel object named `strip`
   - Parameters:
     - **NUM_LEDS**: Number of LEDs (12)
     - **LED_PIN**: GPIO pin for data (4)
     - **NEO_GRB + NEO_KHZ800**: LED type flags for WS2812
   - Global object accessible from any function

#### Setup Function

5. **Serial Communication**
   ```cpp
   Serial.begin(115200);
   ```
   - Initializes serial communication at 115200 baud
   - **Note**: Set Serial Monitor to 115200 baud to view messages

6. **Library Initialization**
   ```cpp
   strip.begin();
   ```
   - Initializes the NeoPixel library
   - Prepares the LED ring for use
   - Must be called before any other NeoPixel functions

7. **Brightness Setting**
   ```cpp
   strip.setBrightness(5);  // 5 out of 255, approximately 2%
   ```
   - Sets brightness to very low value (5/255 ≈ 2%)
   - **Critical**: Low brightness protects ESP32-C3 from excessive current draw
   - WS2812 LEDs can draw up to 60mA each at full brightness
   - 12 LEDs × 60mA = 720mA at full brightness (too much for USB!)
   - Low brightness keeps current draw safe for USB power

8. **Clear Display**
   ```cpp
   strip.clear();
   strip.show();
   ```
   - Clears all LEDs (turns them off)
   - **`show()`**: Required to actually update the physical display
   - You can set multiple LEDs before calling `show()` for efficiency

#### Loop Function

9. **LED Sequence Loop**
   ```cpp
   for (int i = 0; i < NUM_LEDS; i++) {
       // Light up LED i
   }
   ```
   - Loops through all 12 LEDs (0-11)
   - Processes one LED at a time
   - Creates sequential lighting effect

10. **Clear Before Setting**
    ```cpp
    strip.clear();
    ```
    - Clears all LEDs before lighting the next one
    - Ensures only one LED is lit at a time
    - Creates the "moving" effect around the ring

11. **Set LED Color**
    ```cpp
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    ```
    - Sets LED at index `i` to white
    - **`setPixelColor(index, color)`**: Sets individual LED color
    - **`Color(R, G, B)`**: Creates RGB color (255, 255, 255 = white)
    - Index 0-11 corresponds to LEDs around the ring

12. **Update Display**
    ```cpp
    strip.show();
    ```
    - **Required**: Actually updates the physical LED ring
    - Sends data to LEDs via the data pin
    - Must be called after setting colors to see changes

13. **Delay Between LEDs**
    ```cpp
    delay(200);  // 200 milliseconds
    ```
    - Waits 200ms before moving to next LED
    - Creates visible sequence (not too fast, not too slow)
    - Adjust to change speed of sequence

14. **Clear After Sequence**
    ```cpp
    strip.clear();
    strip.show();
    ```
    - Clears display after all LEDs have been lit
    - Creates pause before sequence repeats
    - Makes the pattern more visible

### LED Indexing

WS2812 LEDs are indexed starting from 0:
- **LED 0**: First LED (usually marked on ring)
- **LED 1-11**: Subsequent LEDs around the ring
- **Order**: Usually clockwise from the first LED
- **Total**: 12 LEDs (0-11)

### Color Values

RGB color values range from 0-255:
- **Red**: `Color(255, 0, 0)` - Full red, no green/blue
- **Green**: `Color(0, 255, 0)` - Full green, no red/blue
- **Blue**: `Color(0, 0, 255)` - Full blue, no red/green
- **White**: `Color(255, 255, 255)` - Full red, green, and blue
- **Custom**: `Color(R, G, B)` - Any combination (e.g., `Color(255, 128, 0)` = orange)

## Installation

### Step 1: Install Adafruit NeoPixel Library

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. In the search box, type **"Adafruit NeoPixel"**
4. Find **"Adafruit NeoPixel" by Adafruit** (the official library)
5. Click **Install**
6. Wait for installation to complete

### Step 2: Wire Your LED Ring

Connect your WS2812 LED ring to the ESP32-C3:

1. **V+ (Power)** → Connect to **5V** on ESP32-C3
   - **Warning**: Keep brightness low for USB power
   - For brighter displays, use external 5V power supply

2. **V- (Ground)** → Connect to **GND** on ESP32-C3
   - If using external power, connect both grounds together (common ground)

3. **IN (Data Input)** → Connect to **GPIO 4** (or any GPIO pin)
   - **Important**: Note which pin you use - you'll need to update the code!

**Note**: The OUT pin is for daisy-chaining multiple rings - not needed for single ring.

### Step 3: Configure the Code

**Before uploading**, update the pin number if needed:

```cpp
#define LED_PIN 4;  // Change to match your wiring
```

If you connected the data pin to GPIO 2, change it to:
```cpp
#define LED_PIN 2;
```

### Step 4: Upload Code

1. Copy the code from `led-blink-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Watch your LED ring!

## Usage

After uploading, you should see:

1. **Serial Monitor Output**:
   ```
   LED Ring initialized. Starting blink sequence...
   LED 0 ON
   LED 1 ON
   LED 2 ON
   ...
   LED 11 ON
   ```

2. **Visual Effect**:
   - One LED lights up at a time in white
   - LEDs light sequentially around the ring (0 → 1 → 2 → ... → 11)
   - Each LED stays lit for 200ms
   - After all LEDs have been lit, display clears and sequence repeats

## Safety Notes

⚠️ **CRITICAL Current Draw Warnings:**

### Current Draw Facts

- **Each WS2812 LED**: Up to 60mA at full brightness (white)
- **12-LED ring at full brightness**: Up to 720mA
- **USB power**: Typically 500mA-1A (insufficient for full brightness)
- **ESP32-C3**: Can be damaged by excessive current draw

### Safety Measures

1. **Always set brightness low** (5/255 ≈ 2%) when powered via USB
2. **For brighter displays**: Use external 5V power supply rated for at least 1A
3. **Common ground**: When using external power, connect both grounds together
4. **Never power ring directly** from ESP32-C3's 5V pin at full brightness
5. **Test incrementally**: Start with low brightness, gradually increase if using external power

### Power Supply Recommendations

- **USB power**: Use brightness ≤ 10 (safe for most USB ports)
- **External 5V**: Use power supply rated for at least 1A for 12 LEDs
- **Larger rings**: Need proportionally larger power supplies
- **Check ratings**: Always verify your power supply can handle the current draw

## Troubleshooting

### No LEDs Light Up

- **Check wiring**: Verify V+, V-, and IN connections
- **Check pin number**: Ensure `LED_PIN` matches your wiring
- **Check power**: Verify LED ring is receiving 5V power
- **Check library**: Ensure Adafruit NeoPixel library is installed
- **Check data direction**: Make sure you connected IN (not OUT) pin

### LEDs Light Up But Wrong Order

- **LED indexing**: WS2812 LEDs are indexed 0-11 starting from first LED
- **Physical order**: May differ from logical order - this is normal
- **Test each LED**: Use `setPixelColor(i, Color(255,0,0))` to test individual LEDs

### LEDs Too Dim or Too Bright

- **Brightness setting**: Adjust `setBrightness()` value (5-255)
- **USB power limit**: Keep brightness low (≤10) for USB power
- **External power**: Use external 5V supply for brighter displays
- **Current draw**: Higher brightness = more current draw

### LEDs Flicker or Unstable

- **Power supply**: May be insufficient - try external power supply
- **Wiring**: Check all connections are secure
- **Ground connection**: Ensure good ground connection
- **Data line**: Check data pin connection (may need shorter wire)

### Code Uploads But Nothing Happens

- **Serial Monitor**: Check for error messages
- **Library**: Verify Adafruit NeoPixel library is installed correctly
- **Pin configuration**: Double-check `LED_PIN` matches your wiring
- **Hardware**: Test with multimeter to verify power and connections

### ESP32-C3 Resets or Crashes

- **Current draw**: Too much current - reduce brightness
- **Power supply**: Insufficient power - use external supply
- **USB cable**: May not provide enough current - try different cable
- **Short circuit**: Check for wiring errors

## Customization

### Change Blink Speed

To make the sequence faster or slower:

```cpp
delay(100);  // Faster (100ms)
delay(500);  // Slower (500ms)
```

### Change Colors

To use different colors:

```cpp
// Red
strip.setPixelColor(i, strip.Color(255, 0, 0));

// Green
strip.setPixelColor(i, strip.Color(0, 255, 0));

// Blue
strip.setPixelColor(i, strip.Color(0, 0, 255));

// Orange
strip.setPixelColor(i, strip.Color(255, 128, 0));

// Purple
strip.setPixelColor(i, strip.Color(255, 0, 255));
```

### Change Brightness

To adjust brightness (be careful with USB power!):

```cpp
strip.setBrightness(10);   // Slightly brighter (still safe for USB)
strip.setBrightness(50);   // Medium (use external power)
strip.setBrightness(255);  // Maximum (requires external power!)
```

### Light Multiple LEDs

To light up multiple LEDs at once:

```cpp
for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));  // Light all LEDs
}
strip.show();
```

### Different Patterns

**Counter-clockwise**:
```cpp
for (int i = NUM_LEDS - 1; i >= 0; i--) {
    // Light LEDs in reverse order
}
```

**Alternating**:
```cpp
for (int i = 0; i < NUM_LEDS; i += 2) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));  // Every other LED
}
```

**Chase Effect**:
```cpp
for (int i = 0; i < NUM_LEDS; i++) {
    strip.clear();
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.setPixelColor((i + 1) % NUM_LEDS, strip.Color(128, 128, 128));  // Trail
    strip.show();
    delay(100);
}
```

### Fade Effect

To create a fade effect:

```cpp
for (int brightness = 0; brightness <= 255; brightness++) {
    strip.setBrightness(brightness);
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(10);
}
```

## Next Steps

Once you've verified your LED ring works, try:

- **Epoch Clock**: Display Cardano epoch progress on the ring
- **Color patterns**: Create rainbow effects or color gradients
- **Animations**: Add smooth transitions and effects
- **Multiple rings**: Daisy-chain multiple rings together
- **Sensor integration**: Change colors based on sensor readings
- **Blockchain events**: Light up LEDs based on blockchain events

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/epoch-clock)

## Additional Resources

- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel) - Official library repository
- [WS2812 LED Guide](https://learn.adafruit.com/adafruit-neopixel-uberguide) - Comprehensive guide to WS2812 LEDs
- [NeoPixel Best Practices](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices) - Power and wiring recommendations
- [ESP32-C3 Pinout Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html) - GPIO pin reference

## Notes

- This is a static display test - perfect for verifying setup
- Brightness is intentionally low to protect ESP32-C3
- For production use, consider external power supply for brighter displays
- WS2812 uses special timing protocol - library handles this automatically
- Each LED can display any RGB color independently
