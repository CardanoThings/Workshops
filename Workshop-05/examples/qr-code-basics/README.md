# QR Code Display on TFT_eSPI

Simple QR code generation and display example for TFT displays using sprites

## Overview

This example demonstrates how to generate and display QR codes on a TFT display using the TFT_eSPI library. The QR code is rendered to a sprite (off-screen buffer) first, allowing for flexible positioning and manipulation before displaying it on the screen. This approach is essential for creating payment interfaces where users can scan QR codes with their mobile wallets.

## Requirements

- **ESP32** microcontroller (or compatible ESP32 board like ESP32-C3)
- **TFT display** compatible with TFT_eSPI library (e.g., Cheap Yellow Display / CYD)
- Arduino IDE with ESP32 board support installed
- **Required Libraries**:
  - **QRcodeDisplay** - Base QR code library (must be installed via Library Manager)
  - **QRcode_eSPI** - TFT_eSPI adapter for QR codes (must be installed via Library Manager)
  - **TFT_eSPI** - TFT display library (must be installed and configured for your display)
  - **SPI** - Built-in library for SPI communication
- USB cable for programming and power
- SPI connection between microcontroller and display (built-in for CYD)

## How It Works

### Key Concepts

- **Sprites**: Off-screen buffers that act like virtual canvases. You draw to them in memory first, then push them to the display at any position. This prevents flickering and allows flexible positioning.
- **QR Code Generation**: The QRcode_eSPI library encodes text/URLs into QR codes and draws them to sprites. The library automatically calculates scaling and includes the mandatory "quiet zone" (4-module margin) required by QR code standards.
- **Sprite Positioning**: After generating the QR code in a sprite, you calculate where to position it on the display. The example centers it, but you can position it anywhere.

### Code Structure

#### Libraries

1. **`#include <SPI.h>`**
   - Serial Peripheral Interface library
   - Required for SPI communication with TFT display
   - Usually included automatically, but explicitly included for clarity

2. **`#include <TFT_eSPI.h>`**
   - TFT display library for ESP32
   - **Must be installed** via Library Manager
   - Provides functions for drawing graphics, text, and sprites
   - **Important**: Must be configured for your specific display (CYD users should have User_Setup.h configured)

3. **`#include <qrcode_espi.h>`**
   - QR code library adapter for TFT_eSPI
   - **Must be installed** via Library Manager
   - Provides QR code generation functions that work with TFT_eSPI sprites
   - Depends on QRcodeDisplay library

#### Display and Sprite Objects

4. **Display Object**
   ```cpp
   TFT_eSPI display = TFT_eSPI();
   ```
   - Creates a global display object named `display`
   - All display operations use this object
   - Global so it can be accessed from any function

5. **Sprite Object**
   ```cpp
   TFT_eSprite sprite = TFT_eSprite(&display);
   ```
   - Creates a sprite object linked to the display
   - Sprites are off-screen buffers for drawing graphics
   - Allows drawing to memory first, then pushing to display
   - Reduces flickering and enables flexible positioning

6. **QR Code Generator Object**
   ```cpp
   QRcode_eSPI qrcode(&sprite);
   ```
   - Creates a QR code generator configured to draw to the sprite
   - Takes a pointer to the sprite object
   - All QR code drawing operations go to the sprite, not directly to display

#### Setup Function

7. **Display Initialization**
   ```cpp
   display.begin();
   ```
   - Powers on the display and initializes the driver chip
   - Must be called before any other display functions
   - Configures SPI communication and display hardware

8. **Display Color Inversion**
   ```cpp
   display.invertDisplay(true);
   ```
   - Inverts display colors to correct color mapping
   - **Required for some displays** (like some CYD variants) to show correct colors
   - Without this, colors may appear inverted
   - If colors look wrong, try changing this to `false`

9. **Display Rotation**
   ```cpp
   display.setRotation(0);
   ```
   - Sets display orientation
   - Rotation values:
     - **0**: Portrait (0°)
     - **1**: Landscape (90°) - Common for CYD
     - **2**: Portrait flipped (180°)
     - **3**: Landscape flipped (270°)
   - Adjust based on your display orientation needs

10. **Fill Screen**
    ```cpp
    display.fillScreen(TFT_BLACK);
    ```
    - Fills the entire screen with black background
    - Creates a solid background before drawing QR code
    - Available color constants: `TFT_BLACK`, `TFT_WHITE`, `TFT_RED`, `TFT_GREEN`, `TFT_BLUE`, etc.

11. **QR Code Size Definition**
    ```cpp
    int qrSize = 200;
    ```
    - Defines the size of the QR code sprite in pixels
    - Larger values = larger QR code (easier to scan but uses more memory)
    - Smaller values = smaller QR code (saves memory but may be harder to scan)
    - For CYD displays (240×320), 180-220 pixels works well
    - Adjust based on your display size and memory constraints

12. **Sprite Creation**
    ```cpp
    sprite.createSprite(qrSize, qrSize);
    ```
    - Creates a sprite buffer with specified dimensions (200×200 pixels in this example)
    - Allocates RAM for the sprite
    - **Memory note**: Larger sprites use more RAM, so be mindful of available memory

13. **Sprite Background**
    ```cpp
    sprite.fillSprite(TFT_WHITE);
    ```
    - Fills the sprite with white background
    - **Required**: QR codes need white background for proper contrast
    - Black QR code modules will be drawn on this white background

14. **QR Code Initialization**
    ```cpp
    qrcode.init();
    ```
    - Initializes the QR code generator with the sprite dimensions
    - Automatically calculates the scaling factor based on sprite size
    - Formula: `multiply = spriteSize / WD` (where WD is QR code module width)
    - The QR code will be automatically scaled and centered within the sprite

15. **QR Code Generation**
    ```cpp
    qrcode.create("https://cardanothings.io");
    ```
    - Encodes the string into a QR code
    - Draws the QR code to the sprite buffer
    - The string can be:
      - URLs: `"https://example.com"`
      - Plain text: `"Hello, Cardano!"`
      - Payment URIs: `"web+cardano:addr1q...?amount=1000000"`
      - Any text data (up to QR code capacity limits)

16. **Position Calculation**
    ```cpp
    int spriteX = (display.width() - qrSize) / 2;
    int spriteY = (display.height() - qrSize) / 2;
    ```
    - Calculates position to center the sprite on the display
    - **spriteX**: Horizontal position (centered)
    - **spriteY**: Vertical position (centered)
    - Formula: `(screenSize - spriteSize) / 2`
    - You can modify these values to position the QR code anywhere

17. **Display Sprite**
    ```cpp
    sprite.pushSprite(spriteX, spriteY);
    ```
    - Pushes the sprite buffer to the display at specified coordinates
    - Copies the sprite content to the display
    - Origin (0,0) is at top-left corner of display
    - The QR code appears at the calculated position

#### Loop Function

18. **Empty Loop**
    ```cpp
    void loop() {
        delay(1000);
    }
    ```
    - Empty loop because this is a static QR code
    - Once drawn in `setup()`, nothing needs to update
    - Delay prevents the loop from running too fast
    - For dynamic QR codes, you would regenerate and redraw here

### Sprite System Explained

**What are Sprites?**
- Sprites are off-screen buffers (virtual canvases) stored in RAM
- You draw graphics to sprites first, then push them to the display
- Benefits:
  - **No flickering**: Complete graphics are drawn before displaying
  - **Flexible positioning**: Push sprite to any position on screen
  - **Reusability**: Can reuse sprite buffers for multiple graphics
  - **Manipulation**: Can modify sprite content before displaying

**Sprite Memory Usage:**
- Memory = width × height × bytes per pixel
- For 200×200 sprite with 2 bytes per pixel: 200 × 200 × 2 = 80,000 bytes (~78 KB)
- ESP32 typically has 200-300 KB free RAM, so sprites are feasible
- Larger sprites use more memory - adjust `qrSize` if you get out-of-memory errors

**Sprite Coordinate System:**
- Sprite origin (0,0) is at top-left corner of sprite
- When pushing to display, coordinates specify where sprite's top-left corner appears
- Display origin (0,0) is at top-left corner of display

## Installation

### Step 1: Install QRcodeDisplay Library

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. Search for **"QRcodeDisplay"**
4. Find **"QRcodeDisplay" by yoprogramo** in the list
5. Click **Install**
6. Wait for the installation to complete

**GitHub**: https://github.com/yoprogramo/QRcodeDisplay

### Step 2: Install QRcode_eSPI Library

1. In the same Library Manager window, search for **"QRcode_eSPI"**
2. Find **"QRcode_eSPI" by yoprogramo** in the list
3. Click **Install**
4. Wait for the installation to complete

**GitHub**: https://github.com/yoprogramo/QRcode_eSPI

**Note**: QRcode_eSPI depends on both QRcodeDisplay and TFT_eSPI. Make sure all three libraries are installed.

### Step 3: Install and Configure TFT_eSPI Library

You should already have TFT_eSPI installed from previous workshops. If not:

1. Search for **"TFT_eSPI"** in Library Manager
2. Find **"TFT_eSPI" by Bodmer** and install it
3. **CRITICAL**: Configure TFT_eSPI for your display:
   - **CYD users**: Replace `User_Setup.h` with CYD-specific version from [ESP32-Cheap-Yellow-Display Repository](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/DisplayConfig/User_Setup.h)
   - **Other displays**: Configure according to your display's specifications
4. Restart Arduino IDE after configuration changes

**GitHub**: https://github.com/Bodmer/TFT_eSPI

### Step 4: Upload Code

1. Copy the code from `qr-code-basics.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → [Your Board]**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Check your display - you should see a QR code!

## Usage

After uploading, you should see:

- **Black background** filling the entire screen
- **White square** (sprite background) centered on the screen
- **Black QR code** displayed on the white square
- **Centered QR code** that can be scanned with a smartphone

### Testing the QR Code

1. **Scan with smartphone**:
   - Open your smartphone's camera app (most modern phones have built-in QR scanning)
   - Point camera at the QR code on the display
   - Wait for the QR code to be recognized
   - Tap the notification that appears

2. **Expected result**:
   - The QR code should decode to: `https://cardanothings.io`
   - Your phone should open the URL in a browser

3. **Alternative scanners**:
   - Use a dedicated QR code scanner app if your camera doesn't recognize it
   - Ensure good lighting and hold phone steady
   - Make sure entire QR code is visible in frame

## Customization

### Change QR Code Size

To make the QR code larger or smaller:

```cpp
int qrSize = 150;  // Smaller QR code (saves memory)
int qrSize = 250;  // Larger QR code (easier to scan, uses more memory)
```

**Size recommendations**:
- **CYD (240×320)**: 180-220 pixels works well
- **Smaller displays**: 120-150 pixels
- **Larger displays**: 200-250 pixels

**Memory considerations**:
- 150×150 sprite ≈ 45 KB RAM
- 200×200 sprite ≈ 80 KB RAM
- 250×250 sprite ≈ 125 KB RAM

### Change QR Code Position

To position the QR code elsewhere on the display:

```cpp
// Top-left corner
int spriteX = 0;
int spriteY = 0;

// Top-right corner
int spriteX = display.width() - qrSize;
int spriteY = 0;

// Bottom-left corner
int spriteX = 0;
int spriteY = display.height() - qrSize;

// Bottom-right corner
int spriteX = display.width() - qrSize;
int spriteY = display.height() - qrSize;

// Custom position (e.g., 20 pixels from left, 50 pixels from top)
int spriteX = 20;
int spriteY = 50;
```

### Change QR Code Content

To encode different data:

```cpp
// URL
qrcode.create("https://example.com");

// Plain text
qrcode.create("Hello, Cardano!");

// Payment URI (CIP-13 format)
qrcode.create("web+cardano:addr1q...?amount=1000000");

// WiFi credentials
qrcode.create("WIFI:T:WPA;S:MyNetwork;P:MyPassword;;");

// Contact information (vCard format)
qrcode.create("BEGIN:VCARD\nVERSION:3.0\nFN:John Doe\nEND:VCARD");
```

**Content length limits**:
- QR codes have capacity limits based on error correction level
- Short URLs/text work best
- Very long strings may require higher error correction or may not fit

### Change Display Settings

To customize display appearance:

```cpp
// Invert colors (try false if colors look wrong)
display.invertDisplay(false);

// Different rotation
display.setRotation(1);  // Landscape mode

// Different background color
display.fillScreen(TFT_BLUE);  // Blue background instead of black

// Different sprite background
sprite.fillSprite(TFT_YELLOW);  // Yellow background (not recommended for QR codes)
```

**Note**: QR codes should always have white background and black modules for best scanning results.

### Add Text Labels

To add text above or below the QR code:

```cpp
// After sprite.pushSprite(), add text:
display.setTextColor(TFT_WHITE, TFT_BLACK);
display.setTextSize(2);
display.setCursor(10, 10);
display.println("Scan to pay");

// Or below QR code:
display.setCursor(10, spriteY + qrSize + 10);
display.println("Scan with mobile wallet");
```

### Multiple QR Codes

To display multiple QR codes:

```cpp
// Create first QR code
sprite.fillSprite(TFT_WHITE);
qrcode.create("https://example1.com");
sprite.pushSprite(10, 10);

// Create second QR code
sprite.fillSprite(TFT_WHITE);
qrcode.create("https://example2.com");
sprite.pushSprite(10, 220);
```

**Note**: You can reuse the same sprite for multiple QR codes, just regenerate and push to different positions.

## Troubleshooting

### Display Not Showing QR Code

**Symptoms**: Screen is blank or shows corrupted graphics

**Solutions**:
- **Check TFT_eSPI configuration**: Ensure User_Setup.h is configured for your display
- **Check wiring**: Verify display is properly connected via SPI
- **Check Serial Monitor**: Look for error messages (115200 baud)
- **Try different rotation**: Change `display.setRotation()` values (0-3)
- **Try inverting**: Change `display.invertDisplay(true)` to `false` or vice versa
- **Check power**: Ensure display is receiving adequate power

### QR Code Too Small or Too Large

**Symptoms**: QR code doesn't fit on screen or is too small to scan

**Solutions**:
- **Adjust qrSize**: Change the `qrSize` variable
  - Too small: Increase value (e.g., 200 → 220)
  - Too large: Decrease value (e.g., 200 → 180)
- **Check display dimensions**: Ensure QR code fits within `display.width()` and `display.height()`
- **Verify rotation**: Check that rotation matches your display orientation

### Memory Issues

**Symptoms**: Code fails to compile or ESP32 resets with "Guru Meditation Error"

**Solutions**:
- **Reduce QR code size**: Decrease `qrSize` value
- **Check available memory**: Add `Serial.println(ESP.getFreeHeap());` to see free RAM
- **Close other applications**: Ensure no other memory-intensive code is running
- **Use smaller sprite**: Try 150×150 instead of 200×200

**Memory check code**:
```cpp
Serial.print("Free heap: ");
Serial.println(ESP.getFreeHeap());
```

### QR Code Not Scannable

**Symptoms**: Smartphone camera can't recognize the QR code

**Solutions**:
- **Increase size**: Make QR code larger (at least 150-180 pixels for most displays)
- **Check contrast**: Ensure white background and black modules (library handles this automatically)
- **Check lighting**: Ensure display is well-lit when scanning
- **Check quiet zone**: Library automatically includes quiet zone, but ensure QR code isn't touching screen edges
- **Try different scanner**: Use a dedicated QR scanner app instead of camera
- **Check distance**: Hold phone at appropriate distance (not too close, not too far)

### Compilation Errors

**Symptoms**: Code won't compile, missing library errors

**Solutions**:
- **Install all libraries**: Ensure QRcodeDisplay, QRcode_eSPI, and TFT_eSPI are all installed
- **Check library versions**: Use latest versions from Library Manager
- **Check dependencies**: QRcode_eSPI requires both QRcodeDisplay and TFT_eSPI
- **Verify board selection**: Ensure ESP32 board is selected (not ESP8266 or Arduino)
- **Restart Arduino IDE**: Sometimes IDE needs restart after library installation

### Sprite Not Displaying

**Symptoms**: Screen shows background but no QR code sprite

**Solutions**:
- **Check sprite creation**: Verify `sprite.createSprite()` succeeded
- **Check sprite push**: Ensure `sprite.pushSprite()` is called with correct coordinates
- **Check coordinates**: Verify sprite position is within display boundaries
- **Check sprite background**: Ensure sprite is filled with white before QR code generation
- **Add debug output**: Print sprite dimensions and position to Serial Monitor

**Debug code**:
```cpp
Serial.print("Sprite size: ");
Serial.print(qrSize);
Serial.print(" Position: (");
Serial.print(spriteX);
Serial.print(", ");
Serial.print(spriteY);
Serial.println(")");
```

### Colors Are Wrong

**Symptoms**: QR code appears inverted or wrong colors

**Solutions**:
- **Try inverting**: Change `display.invertDisplay(true)` to `false` or vice versa
- **Check User_Setup.h**: Ensure display configuration is correct
- **Verify sprite colors**: Ensure sprite background is white and QR code modules are black
- **Check display type**: Some displays have different color mappings

## Important Notes

- **Sprite Memory**: Sprites use RAM, so larger sprites require more memory. Monitor available memory if you encounter issues.
- **QR Code Standards**: The library automatically includes the mandatory "quiet zone" (4-module margin) required by QR code standards.
- **Static Display**: This example creates a static QR code that doesn't update. For dynamic QR codes, regenerate in the `loop()` function.
- **Display Configuration**: TFT_eSPI must be properly configured for your display. CYD users must use the CYD-specific User_Setup.h file.
- **Scanning Requirements**: QR codes need good contrast (black on white) and adequate size to be scannable. Ensure proper lighting when scanning.

## Next Steps

Once you've successfully displayed a QR code, try:

- **Dynamic QR codes**: Update QR code content in the `loop()` function
- **Payment URIs**: Create CIP-13 payment URIs for Cardano mobile wallets
- **Multiple QR codes**: Display multiple QR codes on the same screen
- **Add labels**: Add text labels above or below QR codes
- **Combine with web server**: Generate QR codes based on web server requests
- **Payment system**: Build a complete payment interface with QR code generation

## Workshop Documentation

This example is part of Workshop 05: QR-Code Payments. Learn more at:

[View Workshop](https://cardanothings.io/workshops/05-qr-code-payments/qr-code-creation)

## Additional Resources

- [QRcodeDisplay Library GitHub](https://github.com/yoprogramo/QRcodeDisplay) - Base QR code library repository
- [QRcode_eSPI Library GitHub](https://github.com/yoprogramo/QRcode_eSPI) - TFT_eSPI adapter repository
- [TFT_eSPI Library GitHub](https://github.com/Bodmer/TFT_eSPI) - TFT display library with documentation
- [QR Code Standard (ISO/IEC 18004)](https://www.iso.org/standard/62021.html) - Official QR code specification
- [CIP-13 Payment URI Standard](https://cips.cardano.org/cips/cip13/) - Cardano payment URI format
- [ESP32 Memory Management](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/memory-types.html) - Understanding ESP32 memory

## Notes

- This is a static QR code example - perfect for learning the basics
- The `loop()` function is minimal because nothing needs to update
- For dynamic QR codes, regenerate and redraw in the `loop()` function
- Sprites are powerful tools for complex graphics and animations
- QR codes are perfect for mobile payment interfaces and information sharing
- Always test QR code scanning with multiple devices to ensure compatibility
