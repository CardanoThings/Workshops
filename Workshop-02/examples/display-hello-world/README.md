# Hello World Display Test

Simple TFT display test to verify CYD (Cheap Yellow Display) setup with centered text

## Overview

This is a basic "Hello World" test sketch for the Cheap Yellow Display (CYD) with ESP32. It displays centered white text on a blue background, confirming that your TFT display is properly configured and working. This is the first step before building more complex display projects.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **Cheap Yellow Display (CYD)** with built-in TFT display
- Arduino IDE with ESP32 board support installed
- **TFT_eSPI library** (must be installed and configured)
- USB cable for programming and power

## How It Works

### Code Structure

The sketch is very simple:
- **`setup()`**: Initializes display and draws "Hello World!" text
- **`loop()`**: Empty (static display, no updates needed)

### Understanding the Code

#### Libraries

1. **`#include <TFT_eSPI.h>`**
   - TFT display library for ESP32
   - **Must be installed** via Library Manager
   - Provides functions for drawing text, shapes, and colors

2. **`#include <SPI.h>`**
   - Serial Peripheral Interface library
   - Usually included automatically, but explicitly included for clarity
   - Required for SPI communication with TFT display

#### Display Object

3. **TFT Display Object**
   ```cpp
   TFT_eSPI tft = TFT_eSPI();
   ```
   - Creates a global display object named `tft`
   - All display operations use this object
   - Global so it can be accessed from any function

#### Setup Function

4. **Serial Communication**
   ```cpp
   Serial.begin(115200);
   ```
   - Initializes serial communication for debugging
   - **Note**: Set Serial Monitor to 115200 baud to view messages

5. **Display Initialization**
   ```cpp
   tft.init();
   ```
   - Powers on the display and initializes the ILI9341 driver chip
   - Must be called before any other display functions
   - Configures SPI communication and display hardware

6. **Display Rotation**
   ```cpp
   tft.setRotation(1);
   ```
   - Sets landscape orientation
   - Rotation values:
     - **0**: Portrait (0°)
     - **1**: Landscape (90°) - **Recommended for CYD**
     - **2**: Portrait flipped (180°)
     - **3**: Landscape flipped (270°)
   - Rotation 1 gives you 320×240 pixels (width × height)

7. **Color Inversion**
   ```cpp
   tft.invertDisplay(true);
   ```
   - Inverts display colors to correct color mapping
   - **Required for some CYD displays** to show correct colors
   - Without this, colors may appear inverted:
     - Blue might appear yellow
     - White might appear black
   - If colors look wrong, try changing this to `false`

8. **Fill Screen**
   ```cpp
   tft.fillScreen(TFT_BLUE);
   ```
   - Fills the entire screen with blue color
   - Creates a solid background
   - Screen dimensions: 320×240 pixels (in rotation 1)
   - Available color constants: `TFT_BLACK`, `TFT_WHITE`, `TFT_RED`, `TFT_GREEN`, `TFT_BLUE`, `TFT_YELLOW`, `TFT_CYAN`, `TFT_MAGENTA`

9. **Text Color**
   ```cpp
   tft.setTextColor(TFT_WHITE, TFT_BLUE);
   ```
   - Sets text color (foreground) to white
   - Sets background color to blue
   - Format: `setTextColor(foreground, background)`
   - Background color is used when drawing text

10. **Text Size**
    ```cpp
    tft.setTextSize(3);
    ```
    - Sets text size multiplier
    - Base character size: 6 pixels wide × 8 pixels tall
    - Size 3 means: 18 pixels wide × 24 pixels tall (6×3 × 8×3)
    - Larger sizes make text easier to read
    - Valid sizes: 1-7 (larger sizes may not fit on screen)

11. **Text Centering Calculation**
    ```cpp
    String text = "Hello World!";
    int textWidth = text.length() * 6 * 3;   // Approximate width
    int textHeight = 8 * 3;                  // Approximate height
    int textX = (320 - textWidth) / 2;       // Center horizontally
    int textY = (240 - textHeight) / 2;      // Center vertically
    ```
    - Calculates position to center text on screen
    - **Text width formula**: `text.length() × 6 × textSize`
      - Each character is approximately 6 pixels wide (base)
      - Multiplied by text size (3 in this case)
    - **Text height formula**: `8 × textSize`
      - Base height is 8 pixels
      - Multiplied by text size
    - **Centering formula**: `(screenSize - textSize) / 2`
      - Finds leftover space and divides equally
      - Centers horizontally (X) and vertically (Y)

12. **Display Text**
    ```cpp
    tft.setCursor(textX, textY);
    tft.println("Hello World!");
    ```
    - **`setCursor(x, y)`**: Sets where text will be drawn
      - Origin (0,0) is at top-left corner
      - X increases left to right (0-319)
      - Y increases top to bottom (0-239)
    - **`println()`**: Draws text and moves cursor to next line
    - Text appears at calculated centered position

#### Loop Function

13. **Empty Loop**
    ```cpp
    void loop() {
        // Nothing to do in the loop for this test
    }
    ```
    - Empty because this is a static display
    - Once drawn in `setup()`, nothing needs to update
    - For dynamic displays, you would update content here

### Screen Dimensions and Coordinate System

Understanding the display coordinate system is essential:

- **Screen dimensions**: 320 pixels wide × 240 pixels tall (in landscape mode with rotation 1)
- **Origin point**: (0,0) is at the top-left corner of the screen
- **X-axis**: Increases from left (0) to right (319)
- **Y-axis**: Increases from top (0) to bottom (239)

```
(0,0) ──────────────────────── (319,0)
  │                               │
  │                               │
  │         Screen Area           │
  │        320 × 240 pixels       │
  │                               │
  │                               │
(0,239) ──────────────────────── (319,239)
```

### Text Rendering Details

**Text Properties:**
- **Text color**: White foreground, blue background
- **Text size**: 3 (18×24 pixels per character)
- **Character dimensions**: Approximately 6 pixels wide per size unit
- **Cursor positioning**: Top-left corner of text

**Text Size Reference:**
- Size 1: 6×8 pixels (default)
- Size 2: 12×16 pixels
- Size 3: 18×24 pixels (used in this example)
- Size 4: 24×32 pixels
- Larger sizes available but may not fit on screen

## Installation

### Step 1: Install TFT_eSPI Library

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. Search for **"TFT_eSPI"**
4. Find **"TFT_eSPI" by Bodmer** (the one with many downloads)
5. Click **Install**
6. Wait for installation to complete

### Step 2: Configure for Cheap Yellow Display (CYD)

**CRITICAL**: The TFT_eSPI library must be configured specifically for the CYD:

1. Download the CYD-specific `User_Setup.h` file from:
   [ESP32-Cheap-Yellow-Display Repository](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/DisplayConfig/User_Setup.h)

2. Locate your TFT_eSPI library folder:
   - **Windows**: `Documents\Arduino\libraries\TFT_eSPI\`
   - **Mac**: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - **Linux**: `~/Arduino/libraries/TFT_eSPI/`

3. **Replace** the existing `User_Setup.h` file with the downloaded CYD version

4. **Restart Arduino IDE** for changes to take effect

**Important Configuration Details:**
- **Display Driver**: ILI9341_2_DRIVER
- **Display Size**: 240×320 pixels
- **Pin Configuration**: Correct GPIO pins for the CYD
- **SPI Speed**: 55MHz for optimal performance
- **Backlight**: GPIO 21 (controlled automatically)

**⚠️ Warning**: If you skip this configuration step, your display will NOT work correctly!

### Step 3: Upload Code

1. Copy the code from `hello-world-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Watch your display!

## Usage

After uploading, you should see:

- **Blue background** filling the entire screen
- **White text** saying "Hello World!" centered on the screen
- **Large, readable text** (size 3)

The display should remain static (no flickering or updates).

## Troubleshooting

### Display Shows Nothing

- **Check User_Setup.h**: Ensure you replaced it with the CYD version
- **Check connections**: Verify CYD is properly connected to ESP32
- **Check power**: Ensure CYD is receiving power (USB cable)
- **Restart Arduino IDE**: After changing User_Setup.h, IDE must restart

### Colors Are Wrong (Blue Shows as Yellow, White Shows as Black)

- **Try inverting**: Change `tft.invertDisplay(true)` to `tft.invertDisplay(false)`
- **Check User_Setup.h**: Ensure CYD-specific configuration is correct
- **Different CYD versions**: Some CYD variants have different color mappings

### Text Is Not Centered

- **Check rotation**: Ensure `setRotation(1)` is set correctly
- **Verify calculations**: Text centering formula assumes size 3
- **Screen dimensions**: Should be 320×240 in rotation 1

### Text Is Too Small or Too Large

- **Adjust text size**: Change `setTextSize(3)` to a different value (1-7)
- **Recalculate position**: If you change size, recalculate centering
- **Screen space**: Larger sizes may not fit on screen

### Display Shows Corrupted Graphics

- **User_Setup.h**: Most likely cause - ensure CYD version is installed
- **SPI speed**: May need adjustment in User_Setup.h
- **Power issues**: Ensure stable power supply
- **Wiring**: Check SPI connections (though CYD has built-in connections)

### Serial Monitor Shows "Display test complete!" But Screen Is Blank

- **Backlight**: Some CYD displays need backlight control
- **Inversion**: Try changing `invertDisplay()` setting
- **Rotation**: Try different rotation values (0-3)
- **Hardware issue**: Display may be faulty

## Customization

### Change Text

To display different text:

```cpp
String text = "Your Text Here";
// Update centering calculation
int textWidth = text.length() * 6 * 3;
// ... rest of code
tft.println(text);
```

### Change Colors

To use different colors:

```cpp
tft.fillScreen(TFT_RED);  // Red background
tft.setTextColor(TFT_YELLOW, TFT_RED);  // Yellow text on red
```

Available colors: `TFT_BLACK`, `TFT_WHITE`, `TFT_RED`, `TFT_GREEN`, `TFT_BLUE`, `TFT_YELLOW`, `TFT_CYAN`, `TFT_MAGENTA`

### Change Text Size

To make text larger or smaller:

```cpp
tft.setTextSize(4);  // Larger text
// Remember to recalculate centering position!
int textWidth = text.length() * 6 * 4;  // Update multiplier
int textHeight = 8 * 4;
```

### Change Position

To position text elsewhere:

```cpp
tft.setCursor(10, 10);  // Top-left corner
tft.setCursor(10, 220);  // Bottom-left corner
tft.setCursor(160, 120);  // Center (manual)
```

### Multiple Lines of Text

To display multiple lines:

```cpp
tft.setCursor(10, 50);
tft.println("Line 1");
tft.setCursor(10, 80);
tft.println("Line 2");
```

### Different Backgrounds

To create patterns or gradients:

```cpp
// Draw a rectangle instead of filling screen
tft.fillRect(0, 0, 320, 240, TFT_BLUE);

// Draw multiple rectangles for pattern
tft.fillRect(0, 0, 160, 120, TFT_RED);
tft.fillRect(160, 120, 160, 120, TFT_GREEN);
```

## Next Steps

Once you've verified your display works, try:

- **Display wallet balance**: Show Cardano balance on screen
- **Multiple text elements**: Display multiple pieces of information
- **Dynamic updates**: Update display content in the loop
- **Shapes and graphics**: Draw rectangles, circles, lines
- **Images**: Display bitmap images
- **Animations**: Create simple animations

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/display-data)

## Additional Resources

- [TFT_eSPI Library GitHub](https://github.com/Bodmer/TFT_eSPI) - Official repository with documentation
- [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display) - CYD-specific resources and examples
- [TFT_eSPI Arduino Reference](https://www.arduino.cc/reference/en/libraries/tft-espi/) - Official Arduino library documentation
- [Adafruit GFX Graphics Library](https://learn.adafruit.com/adafruit-gfx-graphics-library) - TFT_eSPI is based on this library
- [LVGL Graphics Library](https://github.com/lvgl/lvgl) - Advanced alternative for complex UIs

## Notes

- This is a static display test - perfect for verifying setup
- The `loop()` function is empty because nothing needs to update
- For dynamic displays, you'll update content in the `loop()` function
- Always configure `User_Setup.h` before using TFT_eSPI with CYD
- Screen dimensions are 320×240 pixels in landscape mode (rotation 1)
