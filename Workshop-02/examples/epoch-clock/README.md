# Epoch Clock Code

Display Cardano epoch progress on circular WS2812 LED ring with blue progress indicator and white walking LED (clock second-hand effect)

## Overview

This project combines WiFi connectivity, API calls, and LED ring display to create a physical Epoch Clock that visually represents epoch progress. The circular LED ring with 12 LEDs lights up progressively around the circle to show how far through the current epoch the blockchain has progressed, based on block data from the API.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **WS2812 LED Ring** (12 LEDs recommended)
- Arduino IDE with ESP32 board support installed
- **Required Libraries**:
  - WiFi (built-in for ESP32)
  - HTTPClient (built-in for ESP32)
  - ArduinoJson (must be installed via Library Manager)
  - Adafruit NeoPixel (must be installed via Library Manager)
  - WiFiClientSecure (built-in for ESP32)
- USB cable for programming and power
- **Optional**: External 5V power supply for brighter displays
- WiFi network connection

## How It Works

### Key Concepts

- **Epoch Progress**: Cardano epochs last approximately 5 days. By tracking blocks created within an epoch, we can calculate what percentage of the epoch has been completed.
- **API Integration**: The code uses the Koios API to fetch current blockchain state, including block height and epoch information needed to calculate progress.
- **Visual Progress Indicator**: The 12 LEDs on the circular ring light up sequentially around the circle to create a clock-like visual representation of epoch progress. As more blocks are created, more LEDs light up around the ring.
- **LED Ring Control**: The code directly controls individual RGB LEDs on the WS2812 ring to create a circular progress indicator. Each LED can display any color, allowing for creative visualizations of epoch progression.
- **Non-blocking Timing**: Using `millis()` instead of `delay()` allows the microcontroller to update the display while checking for new blockchain data.

### Program Flow

1. **Initialization (`setup()`)**:
   - Initializes serial communication for debugging
   - Initializes the NeoPixel LED ring
   - Sets brightness to low value (5/255) for safety
   - Connects to WiFi network
   - Performs initial API call to fetch epoch data
   - Displays initial progress on LED ring

2. **Main Loop (`loop()`)**:
   - Checks WiFi connection status and reconnects if needed
   - Every minute (60,000ms), fetches updated epoch data from API
   - Every 5 seconds, updates the walking LED position
   - Continuously updates the display without blocking

3. **Data Fetching (`fetchEpochData()`)**:
   - Connects to Koios API tip endpoint
   - Retrieves current epoch number and epoch slot
   - Parses JSON response using ArduinoJson
   - Detects epoch changes and resets display when new epoch starts

4. **Progress Display (`displayProgress()`)**:
   - Calculates epoch progress percentage based on current slot
   - Maps progress percentage to number of LEDs to light (0-12)
   - Lights up LEDs in blue to show epoch completion
   - Updates physical display

5. **Walking LED (`updateWalkingLED()`)**:
   - Displays epoch progress (blue LEDs) first
   - Adds white LED at current position
   - Moves to next position every 5 seconds
   - Creates clock-like second-hand effect (full rotation in 60 seconds)

### Progress Calculation

The code calculates epoch progress using the following steps:

1. **Total Slots**: Each epoch contains approximately 432,000 slots (defined as `SLOTS_PER_EPOCH`)
2. **Current Slot**: Fetched from API as `epoch_slot` value
3. **Progress Percentage**: `(current_epoch_slot / total_epoch_slots) × 100`
4. **LEDs to Light**: `(progress_percentage / 100) × 12`
5. **Display**: Lights up that many LEDs around the ring in blue

**Example**: If we're at slot 216,000 in an epoch:
- Progress = (216,000 / 432,000) × 100 = 50%
- LEDs to light = (50 / 100) × 12 = 6 LEDs
- Result: 6 LEDs lit around the ring (halfway complete)

### Display Functions

#### `displayProgress()`

Lights up the appropriate number of LEDs around the ring based on calculated epoch progress. All LEDs are lit in blue to create a simple visual progress indicator showing how far through the epoch the blockchain has progressed.

**Process**:
1. Calculates progress percentage from current epoch slot
2. Maps percentage to number of LEDs (0-12)
3. Clears all LEDs
4. Lights up LEDs sequentially from LED 0
5. Updates physical display

#### `updateWalkingLED()`

Displays a white LED that moves around the ring, blinking at each position for 5 seconds. This creates a clock-like second hand effect, completing a full rotation in 60 seconds (1 minute). The white LED appears on top of the blue progress LEDs.

**Process**:
1. Calls `displayProgress()` to show blue progress LEDs
2. Sets white LED at current `walkPosition`
3. Updates display
4. Increments `walkPosition` (wraps around after LED 11)
5. Repeats every 5 seconds

#### `clearDisplay()`

Turns off all LEDs, used when resetting for a new epoch. Called automatically when epoch number changes.

### Code Structure

#### Libraries and Configuration

```cpp
#include <WiFi.h>              // WiFi connectivity
#include <HTTPClient.h>        // HTTP requests
#include <ArduinoJson.h>       // JSON parsing
#include <Adafruit_NeoPixel.h> // LED ring control
#include <WiFiClientSecure.h> // HTTPS support
```

#### Key Variables

- **`LED_PIN`**: GPIO pin connected to LED ring data input (default: 4)
- **`NUM_LEDS`**: Number of LEDs in ring (12)
- **`SLOTS_PER_EPOCH`**: Total slots per epoch (432,000)
- **`checkInterval`**: API fetch interval (60,000ms = 1 minute)
- **`walkInterval`**: Walking LED update interval (5,000ms = 5 seconds)
- **`currentEpoch`**: Current epoch number
- **`currentEpochSlot`**: Current slot within epoch
- **`walkPosition`**: Current position of walking LED (0-11)

#### Timing System

The code uses non-blocking timing with `millis()`:

- **API Updates**: Every 60 seconds (1 minute)
- **Walking LED**: Every 5 seconds
- **Epoch Detection**: Continuous (checks on each API call)

This allows the display to update smoothly while fetching data in the background.

### Epoch Change Detection

When a new epoch starts:

1. `fetchEpochData()` detects `currentEpoch` has changed
2. Compares with `lastEpoch` to detect transition
3. Clears all LEDs
4. Updates `lastEpoch` to current epoch
5. Display resets and begins filling again from start

## Installation

### Step 1: Install Required Libraries

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. Install the following libraries:
   - **ArduinoJson** by Benoit Blanchon
   - **Adafruit NeoPixel** by Adafruit

### Step 2: Wire Your LED Ring

Connect your WS2812 LED ring to the ESP32-C3:

1. **V+ (Power)** → Connect to **5V** on ESP32-C3
   - **Warning**: Keep brightness low for USB power
   - For brighter displays, use external 5V power supply

2. **V- (Ground)** → Connect to **GND** on ESP32-C3
   - If using external power, connect both grounds together (common ground)

3. **IN (Data Input)** → Connect to **GPIO 4** (or any GPIO pin)
   - **Important**: Note which pin you use - update `LED_PIN` if different!

### Step 3: Configure the Code

**Before uploading**, update these settings:

1. **WiFi Credentials**:
   ```cpp
   const char* ssid = "Your SSID";        // Your WiFi network name
   const char* password = "Your Password"; // Your WiFi password
   ```

2. **LED Pin** (if different from GPIO 4):
   ```cpp
   #define LED_PIN 4;  // Change to match your wiring
   ```

3. **API Endpoint** (optional - defaults to Preprod testnet):
   ```cpp
   const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
   // For Mainnet: "https://api.koios.rest/api/v1/tip"
   ```

### Step 4: Upload Code

1. Copy the code from `epoch-clock-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor (115200 baud) to see connection status

## Usage

After uploading, you should see:

1. **Serial Monitor Output**:
   - WiFi connection status
   - Epoch data fetch confirmations

2. **Visual Display**:
   - **Blue LEDs**: Light up progressively around the ring showing epoch progress
   - **White LED**: Moves around the ring every 5 seconds (clock second-hand effect)
   - **Epoch Reset**: When a new epoch starts, display clears and begins filling again

3. **Update Frequency**:
   - Epoch data: Fetched every 60 seconds (1 minute)
   - Walking LED: Updates every 5 seconds
   - Display: Updates continuously

## Understanding the Display

### Blue Progress LEDs

- **Purpose**: Show epoch completion percentage
- **Color**: Blue (`Color(0, 0, 255)`)
- **Behavior**: Light up sequentially from LED 0 as epoch progresses
- **Example**: 6 LEDs lit = 50% epoch complete

### White Walking LED

- **Purpose**: Create clock-like second-hand effect
- **Color**: White (`Color(255, 255, 255)`)
- **Behavior**: Moves to next LED every 5 seconds
- **Timing**: 12 LEDs × 5 seconds = 60 seconds (1 minute) for full rotation
- **Position**: Appears on top of blue progress LEDs

### Epoch Transitions

When a new epoch starts:
- All LEDs clear
- Display resets to 0%
- Progress begins filling again from LED 0

## Troubleshooting

### No LEDs Light Up

- **Check wiring**: Verify V+, V-, and IN connections
- **Check pin number**: Ensure `LED_PIN` matches your wiring
- **Check power**: Verify LED ring is receiving 5V power
- **Check library**: Ensure Adafruit NeoPixel library is installed
- **Check brightness**: Verify brightness is set (may be too low to see)

### WiFi Connection Fails

- **Check credentials**: Verify SSID and password are correct
- **Check network**: Ensure WiFi network is 2.4GHz (ESP32-C3 doesn't support 5GHz)
- **Check signal**: Move closer to router if signal is weak
- **Check Serial Monitor**: Look for connection error messages

### API Requests Fail

- **Check WiFi**: Ensure WiFi connection is active
- **Check endpoint**: Verify API URL is correct
- **Check network**: Ensure internet connection is working
- **Check Serial Monitor**: Look for HTTP error codes

### Display Doesn't Update

- **Check API**: Verify API calls are succeeding (check Serial Monitor)
- **Check timing**: Verify `checkInterval` is not too long
- **Check epoch data**: Verify `currentEpochSlot` is being fetched correctly
- **Check calculation**: Verify progress calculation is working

### Walking LED Doesn't Move

- **Check timing**: Verify `walkInterval` is set correctly (5000ms)
- **Check function**: Ensure `updateWalkingLED()` is being called in `loop()`
- **Check position**: Verify `walkPosition` is incrementing

### Epoch Reset Doesn't Work

- **Check detection**: Verify epoch change detection logic
- **Check comparison**: Ensure `lastEpoch` is being updated
- **Check display**: Verify `clearDisplay()` is being called

## Customization

### Change Update Intervals

**API Update Frequency**:
```cpp
const unsigned long checkInterval = 30000;  // Every 30 seconds
const unsigned long checkInterval = 120000; // Every 2 minutes
```

**Walking LED Speed**:
```cpp
const unsigned long walkInterval = 2000;   // Every 2 seconds (faster)
const unsigned long walkInterval = 10000;  // Every 10 seconds (slower)
```

### Change Colors

**Progress LEDs**:
```cpp
// Green progress
strip.setPixelColor(i, strip.Color(0, 255, 0));

// Red progress
strip.setPixelColor(i, strip.Color(255, 0, 0));

// Rainbow gradient
strip.setPixelColor(i, strip.Color(255 - (i * 21), i * 21, 0));
```

**Walking LED**:
```cpp
// Yellow walking LED
strip.setPixelColor(walkPosition, strip.Color(255, 255, 0));

// Cyan walking LED
strip.setPixelColor(walkPosition, strip.Color(0, 255, 255));
```

### Change Starting Position

To start progress from a different LED position (e.g., 12 o'clock):

```cpp
// Offset LEDs by 3 positions (quarter turn)
int ledIndex = (i + 3) % NUM_LEDS;
strip.setPixelColor(ledIndex, strip.Color(0, 0, 255));
```

### Add Color Coding

Color-code progress by epoch stage:

```cpp
void displayProgress() {
    int progressPercent = (currentEpochSlot * 100) / SLOTS_PER_EPOCH;
    int ledsToLight = (progressPercent * NUM_LEDS) / 100;
    
    strip.clear();
    
    for (int i = 0; i < ledsToLight; i++) {
        // Green for early epoch (0-33%)
        if (i < NUM_LEDS / 3) {
            strip.setPixelColor(i, strip.Color(0, 255, 0));
        }
        // Yellow for middle epoch (33-66%)
        else if (i < (NUM_LEDS * 2) / 3) {
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        }
        // Red for late epoch (66-100%)
        else {
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        }
    }
    
    strip.show();
}
```

### Add Epoch Transition Animation

Add a flash effect when epoch changes:

```cpp
if (currentEpoch != lastEpoch) {
    // Flash all LEDs white
    for (int flash = 0; flash < 3; flash++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, strip.Color(255, 255, 255));
        }
        strip.show();
        delay(200);
        strip.clear();
        strip.show();
        delay(200);
    }
    lastEpoch = currentEpoch;
}
```

### Change Network (Mainnet vs Testnet)

**Preprod Testnet** (default):
```cpp
const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
```

**Mainnet**:
```cpp
const char* apiUrl = "https://api.koios.rest/api/v1/tip";
```

**Preview Testnet**:
```cpp
const char* apiUrl = "https://preview.koios.rest/api/v1/tip";
```

## Safety Notes

⚠️ **Current Draw Warnings:**

- **Brightness**: Set to low value (5/255) for USB power safety
- **Power Supply**: Use external 5V supply for brighter displays
- **Current Rating**: 12 LEDs can draw up to 720mA at full brightness
- **USB Limit**: Most USB ports provide 500mA-1A (insufficient for full brightness)

## Next Steps

Once your Epoch Clock is working, try:

- **Color Patterns**: Add color gradients or rainbow effects
- **Animations**: Add smooth transitions and effects
- **Multiple Networks**: Display progress from different networks
- **Epoch History**: Track and display epoch transition history
- **Custom Enclosures**: Design and build a professional enclosure
- **Remote Monitoring**: Add web interface for remote monitoring

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/epoch-clock)

## Additional Resources

- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel) - Official library repository
- [ArduinoJson Library](https://arduinojson.org/) - JSON parsing library documentation
- [Koios API Documentation](https://preprod.koios.rest/) - API endpoint reference
- [WS2812 LED Guide](https://learn.adafruit.com/adafruit-neopixel-uberguide) - Comprehensive guide to WS2812 LEDs
- [Cardano Epochs and Slots](https://docs.cardano.org/cardano-testnets/about-testnets) - Understanding Cardano epochs

## Notes

- This code uses Preprod testnet by default - change API URL for Mainnet
- Brightness is intentionally low to protect ESP32-C3 from excessive current draw
- For production use, consider external power supply for brighter displays
- WS2812 uses special timing protocol - library handles this automatically
- Each LED can display any RGB color independently
- Epoch progress updates every minute - adjust `checkInterval` for different update rates
