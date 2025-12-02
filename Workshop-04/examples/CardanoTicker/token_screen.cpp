/**
 * token_screen.cpp - Token Holdings Screen
 *
 * This screen displays all the Cardano tokens you own, showing:
 * - Token ticker symbol (e.g., "MIN", "HOSKY")
 * - Amount you own
 * - Total value in USD
 * - 24-hour price change percentage (green if up, red if down)
 *
 * Tokens are custom assets on Cardano blockchain. Unlike ADA (the native
 * currency), tokens are created by projects and can represent anything
 * (governance tokens, meme coins, utility tokens, etc.).
 */

#include "token_screen.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

// External reference to TFT display (defined in main .ino file)
extern TFT_eSPI tft;

/**
 * Draw the token positions screen
 *
 * This function renders a table showing all your token holdings.
 * Each row shows one token with its ticker, amount, value, and price change.
 */
void drawTokenScreen() {
  // Draw header with title and page indicator
  // activeIndex = 1 means this is the second screen (0-indexed)
  renderHeader("Token Positions", 1);

  // Clear the content area (erases previous screen's content)
  clearContentArea();

  // Set default text color (white on black)
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Start drawing below the header
  // kHeaderHeight is the height of the header (34px), +5px for spacing
  int y = kHeaderHeight + 5;

  // Draw screen title with token count
  tft.setTextSize(2);                                   // Larger text for title
  tft.setCursor(10, y);                                 // 10px from left edge
  tft.print("Tokens(" + String(getTokenCount()) + ")"); // e.g., "Tokens(5)"
  y += 35; // Move down for next line

  // Get token count (already limited to MAX_DISPLAY_ITEMS = 8)
  const int tokenCount = getTokenCount();
  const int displayCount = tokenCount; // We can display all tokens (max 8)

  // Switch to smaller text for the table
  tft.setTextSize(1);

  // Draw column headers
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK); // Gray text for headers

  tft.setTextSize(1);
  tft.setCursor(10, y); // "Ticker" column
  tft.print("Ticker");
  tft.setCursor(60, y); // "Amount" column
  tft.print("Amount");
  tft.setCursor(160, y); // "Value" column
  tft.print("Value");
  tft.setCursor(240, y); // "24h Change" column
  tft.print("24h Change");
  y += 16;                                // Move down to start data rows
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Back to white for data

  // Loop through each token and draw a row
  for (int i = 0; i < displayCount; ++i) {
    // Get token data from data fetcher
    TokenInfo token = getToken(i);

    // Truncate token name if too long (so it fits on screen)
    String displayName = token.ticker;
    if (displayName.length() > 20) {
      // If longer than 20 characters, show first 15 + "..."
      displayName = displayName.substring(0, 15) + "...";
    }

    // Draw token ticker (left column)
    tft.setCursor(10, y);
    tft.print(displayName);

    // Draw amount you own (second column)
    tft.setCursor(60, y);
    tft.print(token.amount, 2); // Print with 2 decimal places

    // Draw total value in USD (third column)
    tft.setCursor(160, y);
    tft.print("$" + String(token.value, 2)); // e.g., "$123.45"

    // Draw 24-hour price change (fourth column)
    tft.setCursor(240, y);

    // Color code: green for positive change, red for negative
    if (token.change24h >= 0) {
      tft.setTextColor(TFT_GREEN, TFT_BLACK); // Green = price went up
    } else {
      tft.setTextColor(TFT_RED, TFT_BLACK); // Red = price went down
    }

    // Show change with + or - sign
    // Note: String() already includes the minus sign for negative numbers
    // So we only need to add "+" for positive numbers
    // Example: String(5.67, 2) = "5.67" (we add "+" to make "+5.67")
    //          String(-5.67, 2) = "-5.67" (already has minus, no need to add
    //          "-")
    tft.print((token.change24h >= 0 ? "+" : "") + String(token.change24h, 2));
    tft.print("%");

    // Reset text color back to white
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // Move down for next row
    y += 16;

    // Safety check: stop if we're running out of screen space
    // Don't draw over the ticker at the bottom
    if (y > tft.height() - kTickerHeight - 10) {
      break; // Exit loop early
    }
  }

  // If there are more tokens than we can display, show a message
  // (This shouldn't happen since we limit to 8, but good to have)
  if (tokenCount > displayCount) {
    y += 4; // Add some spacing
    tft.setCursor(10, y);
    tft.print("... and ");
    tft.print(tokenCount - displayCount);
    tft.print(" more");
  }
}
