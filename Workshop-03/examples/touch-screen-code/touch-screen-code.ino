// Include TFT display library and SPI communication
#include <TFT_eSPI.h>
#include <SPI.h>

// Create TFT display object
TFT_eSPI tft = TFT_eSPI();

// Touch screen pin configuration (adjust for your specific display)
#define TOUCH_CS 2    // Chip Select pin for touch controller
#define TOUCH_IRQ 15  // Interrupt pin for touch controller

// Track button press state to prevent multiple triggers
bool mintButtonPressed = false;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize TFT display
    tft.init();
    tft.setRotation(1);        // Set to landscape orientation
    tft.fillScreen(TFT_BLACK);  // Clear screen with black background

    // Draw the mint button on screen
    drawMintButton();
}

void loop() {
    // Variables to store touch coordinates
    uint16_t x, y;

    // Check if screen is being touched
    // getTouch() returns true if touch detected and fills x, y with coordinates
    bool pressed = tft.getTouch(&x, &y);

    if (pressed) {
        // Check if touch coordinates are within button area
        // Button area: x from 50 to 190, y from 100 to 140
        if (x > 50 && x < 190 && y > 100 && y < 140) {
            // Only trigger if button wasn't already pressed (debouncing)
            if (!mintButtonPressed) {
                mintButtonPressed = true;
                triggerMint();  // Call mint function
                delay(500);     // Debounce delay to prevent multiple triggers
            }
        }
    } else {
        // Reset button state when touch is released
        mintButtonPressed = false;
    }
}

void drawMintButton() {
    // Draw button background (filled blue rectangle)
    // Parameters: x, y, width, height, color
    tft.fillRect(50, 100, 140, 40, TFT_BLUE);

    // Draw button border (white rectangle outline)
    tft.drawRect(50, 100, 140, 40, TFT_WHITE);

    // Draw button text "MINT NFT"
    tft.setTextSize(2);                    // Medium text size
    tft.setTextColor(TFT_WHITE, TFT_BLUE);  // White text on blue background
    tft.setCursor(70, 110);                // Position text in center of button
    tft.println("MINT NFT");
}

void triggerMint() {
    // This function is called when the mint button is pressed
    Serial.println("Mint button pressed!");

    // TODO: Add your API call here to send mint request
    // You would typically:
    // 1. Read sensor data
    // 2. Create JSON payload with sensor data
    // 3. Send POST request to your API or NMKR
    // 4. Display success/error message on screen
}