/**
 * screen_helper.cpp - Implementation of screen rendering utilities
 *
 * This file implements the header rendering and content area clearing
 * functions. It uses "sprites" for smooth rendering - sprites are off-screen
 * buffers that we draw to, then push to the display all at once (reduces
 * flicker).
 */

#include "screen_helper.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// External reference to the TFT display (defined in main .ino file)
extern TFT_eSPI tft;

// Total number of screens in rotation
// Update this if you add/remove screens
const uint8_t TOTAL_SCREENS = 4; // Wallet, Tokens, NFTs, Status

// Private namespace - internal constants and variables
namespace {
// Page indicator styling constants (in pixels)
constexpr int kIndicatorRadius = 3;  // Radius of the dots (3px = small dot)
constexpr int kIndicatorSpacing = 3; // Space between dots
constexpr int kIndicatorMargin = 5;  // Margin from screen edge

// Sprite for header rendering
// A sprite is an off-screen buffer - we draw to it, then push it to display
// This reduces flicker compared to drawing directly to the screen
TFT_eSprite headerSprite = TFT_eSprite(&tft);
bool headerSpriteInitialized = false;

/**
 * Ensure the header sprite is created and properly sized
 *
 * Sprites need to be recreated if the display width changes (shouldn't happen,
 * but good to be safe). This function checks and recreates if needed.
 */
void ensureHeaderSprite() {
  // Check if sprite needs to be created or resized
  if (!headerSpriteInitialized || headerSprite.width() != tft.width()) {
    // If sprite exists but is wrong size, delete it first
    if (headerSpriteInitialized) {
      headerSprite.deleteSprite();
    }

    // Create new sprite with correct dimensions
    headerSprite.setColorDepth(16); // 16-bit color (65,536 colors)
    headerSprite.createSprite(tft.width(), kHeaderHeight);
    headerSpriteInitialized = true;
  }
}
} // namespace

/**
 * Render the header bar with title and page indicators
 *
 * This function draws:
 * 1. Screen title on the left
 * 2. A horizontal line separator
 * 3. Page indicator dots on the right (showing which screen you're on)
 *
 * @param title The screen title to display (e.g., "Wallet", "Token Positions")
 * @param activeIndex Which screen is active (0-3) - this dot will be filled
 */
void renderHeader(const char *title, uint8_t activeIndex) {
  // Make sure sprite is ready
  ensureHeaderSprite();

  // Clear sprite with black background
  headerSprite.fillSprite(TFT_BLACK);

  // Set text color (white text on black background)
  headerSprite.setTextColor(TFT_WHITE, TFT_BLACK);
  headerSprite.setTextSize(1); // Small text size

  // Draw title on the left side
  headerSprite.setCursor(5, 6); // X=5px from left, Y=6px from top
  if (title != nullptr) {
    headerSprite.print(title);
  }

  // Draw horizontal line separator below title
  // drawFastHLine draws a horizontal line: (x, y, width, color)
  headerSprite.drawFastHLine(5, 20, headerSprite.width() - 10, TFT_WHITE);

  // Draw page indicator dots on the right side
  if (TOTAL_SCREENS > 0) {
    // Calculate total width needed for all dots
    // Each dot is radius*2 wide, plus spacing between them
    const int totalWidth =
        TOTAL_SCREENS * (kIndicatorRadius * 2) + // Width of all dots
        (static_cast<int>(TOTAL_SCREENS) - 1) *
            kIndicatorSpacing; // Spacing between

    // Calculate starting X position (right-aligned with margin)
    int startX = headerSprite.width() - kIndicatorMargin - totalWidth;

    // Safety check: don't draw off-screen
    if (startX < kIndicatorMargin) {
      startX = kIndicatorMargin;
    }

    const int centerY = 10; // Vertical center of header

    // Draw each page indicator dot
    for (uint8_t i = 0; i < TOTAL_SCREENS; ++i) {
      // Calculate X position of this dot's center
      const int cx = startX + kIndicatorRadius +
                     i * ((kIndicatorRadius * 2) + kIndicatorSpacing);

      // Draw the dot
      if (i == activeIndex) {
        // Active screen: filled white circle (solid dot)
        headerSprite.fillCircle(cx, centerY, kIndicatorRadius, TFT_WHITE);
      } else {
        // Inactive screen: filled black circle (hollow dot)
        headerSprite.fillCircle(cx, centerY, kIndicatorRadius, TFT_BLACK);
      }

      // Draw circle outline for all dots (makes them visible)
      headerSprite.drawCircle(cx, centerY, kIndicatorRadius, TFT_WHITE);
    }
  }

  // Push the sprite to the display
  // This updates the screen all at once, reducing flicker
  headerSprite.pushSprite(0, 0);
}

/**
 * Clear the content area between header and ticker
 *
 * This function erases the middle section of the screen so new content can
 * be drawn without leftover text from the previous screen.
 *
 * Screen layout:
 * [Header - 34px]
 * [Content Area - variable height] <- This is what we clear
 * [Ticker - 30px]
 */
void clearContentArea() {
  // Calculate where content area starts (right below header)
  const int top = kHeaderHeight;

  // Calculate content area height
  // Total screen height minus header and ticker heights
  const int height = tft.height() - kHeaderHeight - kTickerHeight;

  // Safety check: don't try to clear if height is invalid
  if (height <= 0) {
    return;
  }

  // Fill the content area with black (erases everything)
  // fillRect(x, y, width, height, color)
  // x=0 means start at left edge, width=tft.width() means full width
  tft.fillRect(0, top, tft.width(), height, TFT_BLACK);
}
