#include "ticker.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// External reference to the TFT display object (defined in main .ino file)
extern TFT_eSPI tft;

// Create sprite for smooth scrolling (reduces flicker)
// Sprite is an off-screen buffer that can be drawn to screen
TFT_eSprite scrollSprite = TFT_eSprite(&tft);

// Scroll area configuration
const int scrollAreaHeight = 30; // Height of the scrolling area at bottom
const int yPos = 4;        // Y position within the sprite (vertical offset)
const int scrollSpeed = 2; // Pixels to scroll per update

// Token data array
TokenData tokens[] = {{"ADA", 0.3864, -7.89},
                      {"MIN", 0.0123, 2.45},
                      {"HOSKY", 0.0001, -15.23},
                      {"IAG", 0.0456, 5.67}};

// Scrolling variables
int scrollX = 0;      // Current scroll position in pixels
int contentWidth = 0; // Total width of all content for seamless looping

// Calculate total width of all content dynamically
// This function uses actual text width measurements for accurate spacing
void calculateContentWidth() {
  contentWidth = 0;

  // Calculate width for all tokens
  for (int i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
    // Ticker width (size 2)
    tft.setTextSize(2);
    contentWidth += tft.textWidth(tokens[i].ticker) + 4; // Add 4px spacing

    // Price width (size 1)
    tft.setTextSize(1);
    String priceStr = "$" + String(tokens[i].price, 4);
    contentWidth += tft.textWidth(priceStr) + 4; // Add 4px spacing

    // 24h change width (size 1)
    String changeStr = (tokens[i].change24h >= 0 ? "+" : "") +
                       String(tokens[i].change24h, 2) + "%";
    contentWidth +=
        tft.textWidth(changeStr) + 8; // Add 8px spacing after change
  }
}

// Draw all tokens at a given x position
// This function draws content into the sprite buffer
void drawContentLine(int xPos) {
  // Draw all tokens
  for (int i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
    // Draw token ticker (larger text)
    scrollSprite.setTextSize(2);
    scrollSprite.setTextColor(TFT_WHITE, TFT_BLACK);
    scrollSprite.drawString(tokens[i].ticker, xPos, yPos);
    xPos += scrollSprite.textWidth(tokens[i].ticker) +
            4; // Advance position with spacing

    // Draw token price (smaller text, slightly offset vertically)
    scrollSprite.setTextSize(1);
    String priceStr = "$" + String(tokens[i].price, 4);
    scrollSprite.drawString(priceStr, xPos,
                            yPos + 2); // +2 for vertical alignment
    xPos +=
        scrollSprite.textWidth(priceStr) + 4; // Advance position with spacing

    // Draw 24h change with color coding
    String changeStr = (tokens[i].change24h >= 0 ? "+" : "") +
                       String(tokens[i].change24h, 2) + "%";

    // Color: green for positive, red for negative
    if (tokens[i].change24h >= 0) {
      scrollSprite.setTextColor(TFT_GREEN, TFT_BLACK);
    } else {
      scrollSprite.setTextColor(TFT_RED, TFT_BLACK);
    }
    scrollSprite.drawString(changeStr, xPos,
                            yPos + 2); // +2 for vertical alignment
    xPos +=
        scrollSprite.textWidth(changeStr) + 8; // Advance position with spacing
  }
}

// Initialize the ticker display
void initTicker() {
  // Fill screen with black background
  tft.fillScreen(TFT_BLACK);

  // Configure sprite for smooth scrolling
  // 16-bit color depth for full color support
  scrollSprite.setColorDepth(16);

  // Create sprite with full screen width and scroll area height
  // This creates an off-screen buffer for the scrolling content
  scrollSprite.createSprite(tft.width(), scrollAreaHeight);

  // Calculate total content width for seamless scrolling
  calculateContentWidth();

  Serial.println("Token scroll display initialized!");
}

// Update the ticker display (call this in loop)
void updateTicker() {
  // Clear the sprite with black background
  scrollSprite.fillSprite(TFT_BLACK);

  // Draw first copy of content at current scroll position
  drawContentLine(-scrollX);

  // Draw second copy for seamless endless scrolling
  // Offset by contentWidth to create continuous loop
  drawContentLine(-scrollX + contentWidth);

  // Push sprite to display at bottom of screen
  // This updates the display in one operation, reducing flicker
  scrollSprite.pushSprite(0, tft.height() - scrollAreaHeight);

  // Update scroll position
  scrollX += scrollSpeed;

  // Reset scroll position when content has scrolled completely
  // This creates endless scrolling effect
  if (scrollX >= contentWidth) {
    scrollX = 0;
  }

  // Delay to control scroll speed
  delay(30);
}
