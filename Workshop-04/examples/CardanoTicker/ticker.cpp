/**
 * ticker.cpp - Scrolling Token Ticker
 * 
 * This file implements a scrolling ticker at the bottom of the screen that
 * displays token prices continuously. Think of it like a stock market ticker
 * - it scrolls horizontally showing token symbols, prices, and 24h changes.
 * 
 * How it works:
 * 1. We draw all token information into an off-screen buffer (sprite)
 * 2. We draw the content twice (side by side) to create seamless looping
 * 3. We scroll the viewport left, and when we reach the end, we loop back
 * 4. This creates an endless scrolling effect
 * 
 * Technical concepts:
 * - Sprite: An off-screen buffer we draw to, then push to screen all at once
 * - This reduces flicker compared to drawing directly to the screen
 * - Seamless looping: Drawing content twice so when one copy scrolls off,
 *   the second copy is already visible, creating infinite scroll
 */

#include "ticker.h"
#include "data_fetcher.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// External reference to TFT display (defined in main .ino file)
extern TFT_eSPI tft;

// Create sprite for smooth scrolling
// A sprite is an off-screen buffer - we draw to it, then push it to the display
// This reduces flicker because we update the whole area at once
TFT_eSprite scrollSprite = TFT_eSprite(&tft);

// Scroll area configuration
const int scrollAreaHeight = 30;  // Height of ticker area at bottom (in pixels)
                                   // Must match kTickerHeight in screen_helper.h
const int yPos = 4;                // Vertical position within sprite (4px from top)
const int scrollSpeed = 2;         // How many pixels to scroll per update
                                   // Higher = faster scroll, Lower = slower scroll

// Scrolling state variables
int scrollX = 0;      // Current horizontal scroll position (in pixels)
                      // This tracks how far we've scrolled to the left
int contentWidth = 0; // Total width of all token content (in pixels)
                      // Used to calculate when to loop back to start

/**
 * Calculate the price per token
 * 
 * The data fetcher gives us total value (value) and amount owned (amount).
 * To show price per token, we divide: price = total_value / amount
 * 
 * @param token The token information structure
 * @return Price per token in USD, or 0 if amount is 0
 * 
 * Example: If you own 100 tokens worth $50 total, price per token = $0.50
 */
static float getTokenPrice(const TokenInfo& token) {
  // Avoid division by zero (if amount is 0, return 0)
  return (token.amount > 0.0f) ? (token.value / token.amount) : 0.0f;
}

/**
 * Calculate total width of all token content
 * 
 * This function measures how wide all the token information is when displayed.
 * We need this to know when to loop the scroll back to the beginning.
 * 
 * We measure each piece of text individually because different text sizes
 * and lengths take up different amounts of space.
 */
void calculateContentWidth() {
  contentWidth = 0;  // Start with zero width
  const int tokenCount = getTokenCount();

  // Loop through each token and measure its width
  for (int i = 0; i < tokenCount; i++) {
    TokenInfo token = getToken(i);
    float price = getTokenPrice(token);

    // Measure ticker symbol width (larger text, size 2)
    tft.setTextSize(2);
    contentWidth += tft.textWidth(token.ticker) + 4;  // Add 4px spacing after ticker

    // Measure price width (smaller text, size 1)
    tft.setTextSize(1);
    String priceStr = "$" + String(price, 4);  // Format: "$0.1234"
    contentWidth += tft.textWidth(priceStr) + 4;  // Add 4px spacing after price

    // Measure 24h change width (smaller text, size 1)
    // Note: String() already includes the minus sign for negative numbers
    String changeStr = (token.change24h >= 0 ? "+" : "") +
                       String(token.change24h, 2) + "%";  // Format: "+5.67%" or "-2.34%"
    contentWidth += tft.textWidth(changeStr) + 8;  // Add 8px spacing after change (extra space)
  }
  
  // Now contentWidth = total width needed to display all tokens
  // We'll draw this content twice (side by side) for seamless looping
}

/**
 * Draw all token information at a given horizontal position
 * 
 * This function draws all tokens into the sprite buffer starting at xPos.
 * We call this twice with different xPos values to create seamless looping.
 * 
 * @param xPos The horizontal position to start drawing at (can be negative)
 * 
 * What we draw for each token:
 * 1. Ticker symbol (large text, e.g., "MIN")
 * 2. Price per token (small text, e.g., "$0.0123")
 * 3. 24h change (small text, colored green/red, e.g., "+5.67%")
 */
void drawContentLine(int xPos) {
  const int tokenCount = getTokenCount();
  
  // Draw each token in sequence
  for (int i = 0; i < tokenCount; i++) {
    TokenInfo token = getToken(i);
    float price = getTokenPrice(token);
    
    // Draw token ticker symbol (larger, more prominent)
    scrollSprite.setTextSize(2);  // Size 2 = larger text
    scrollSprite.setTextColor(TFT_WHITE, TFT_BLACK);
    scrollSprite.drawString(token.ticker, xPos, yPos);  // Draw at current position
    xPos += scrollSprite.textWidth(token.ticker) + 4;  // Move xPos right by ticker width + spacing

    // Draw token price (smaller text, slightly lower for visual alignment)
    scrollSprite.setTextSize(1);  // Size 1 = smaller text
    String priceStr = "$" + String(price, 4);  // Format: "$0.1234"
    scrollSprite.drawString(priceStr, xPos, yPos + 2);  // +2px down for alignment
    xPos += scrollSprite.textWidth(priceStr) + 4;  // Move xPos right

    // Draw 24h price change with color coding
    // Format: "+5.67%" for positive, "-2.34%" for negative
    // Note: String() already includes the sign for negative numbers
    String changeStr = (token.change24h >= 0 ? "+" : "") +
                       String(token.change24h, 2) + "%";  // Format: "+5.67%" or "-2.34%"

    // Color code: green = price went up, red = price went down
    if (token.change24h >= 0) {
      scrollSprite.setTextColor(TFT_GREEN, TFT_BLACK);  // Green for gains
    } else {
      scrollSprite.setTextColor(TFT_RED, TFT_BLACK);    // Red for losses
    }
    scrollSprite.drawString(changeStr, xPos, yPos + 2);  // Draw change
    xPos += scrollSprite.textWidth(changeStr) + 8;  // Move xPos right (extra spacing)
    
    // After this loop, xPos has moved to the right of all tokens
    // This is how we know where to draw the second copy for seamless looping
  }
}

/**
 * Initialize the ticker display
 * 
 * This function sets up the scrolling ticker. It's called once at startup
 * in setup(). It creates the sprite buffer and calculates content width.
 */
void initTicker() {
  // Fill entire screen with black (clean slate)
  tft.fillScreen(TFT_BLACK);

  // Configure sprite for smooth scrolling
  // 16-bit color depth = 65,536 colors (full color support)
  // Higher color depth = better quality but uses more memory
  scrollSprite.setColorDepth(16);

  // Create the sprite buffer
  // Width = full screen width, Height = ticker area height (30px)
  // This creates an off-screen buffer we can draw to
  scrollSprite.createSprite(tft.width(), scrollAreaHeight);

  // Calculate how wide all the token content is
  // This tells us when to loop the scroll back to the beginning
  calculateContentWidth();

  Serial.println("Token scroll display initialized!");
}

/**
 * Update the ticker display (call this in loop)
 * 
 * This function is called repeatedly from loop() to create the scrolling
 * animation. Each call:
 * 1. Clears the sprite
 * 2. Draws content at current scroll position
 * 3. Draws content again (offset) for seamless looping
 * 4. Pushes sprite to screen
 * 5. Updates scroll position
 * 
 * Seamless looping trick:
 * - We draw the content twice, side by side
 * - When the first copy scrolls off the left, the second copy is already visible
 * - When we reach the end, we reset scrollX to 0 and it looks continuous
 */
void updateTicker() {
  // Clear the sprite buffer with black (erase previous frame)
  scrollSprite.fillSprite(TFT_BLACK);

  // Draw first copy of content
  // -scrollX means we're scrolling left (negative X moves content left)
  // Example: if scrollX = 50, we draw at x = -50, which shows content
  //          that's 50 pixels to the left (already scrolled)
  drawContentLine(-scrollX);

  // Draw second copy of content for seamless looping
  // We offset it by contentWidth so it appears right after the first copy
  // When first copy scrolls off left, second copy is already visible
  drawContentLine(-scrollX + contentWidth);

  // Push the sprite to the display
  // This updates the screen all at once (reduces flicker)
  // Position: x=0 (left edge), y=bottom of screen minus ticker height
  scrollSprite.pushSprite(0, tft.height() - scrollAreaHeight);

  // Update scroll position (move left by scrollSpeed pixels)
  scrollX += scrollSpeed;

  // Check if we've scrolled past all the content
  // If so, reset to 0 to create the loop effect
  if (scrollX >= contentWidth) {
    scrollX = 0;  // Loop back to beginning
  }

  // Small delay to control scroll speed
  // Without this, scrolling would be too fast
  // 30ms = ~33 updates per second (smooth animation)
  delay(30);
}
