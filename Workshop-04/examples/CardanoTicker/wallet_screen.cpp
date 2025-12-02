/**
 * wallet_screen.cpp - Wallet Balance Screen
 * 
 * This screen displays your Cardano wallet information:
 * - ADA balance (your main cryptocurrency holdings)
 * - Stake address (your wallet's staking address)
 * - Last update time (when balance was last fetched)
 * 
 * Cardano Concepts:
 * - ADA: The native cryptocurrency of Cardano (like Bitcoin for Bitcoin network)
 * - Stake Address: A special address used for staking (earning rewards)
 *   Format: starts with "stake1..." (mainnet) or "stake_test1..." (testnet)
 * - Staking: Locking ADA to help secure the network and earn rewards
 */

#include "wallet_screen.h"
#include "config.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

// External reference to TFT display
extern TFT_eSPI tft;

/**
 * Draw the wallet balance screen
 * 
 * This is the first screen shown (index 0). It displays your ADA balance
 * prominently, along with your stake address and last update time.
 */
void drawWalletScreen() {
  // Draw header with title and page indicator
  // activeIndex = 0 means this is the first screen
  renderHeader("Wallet", 0);
  
  // Clear the content area
  clearContentArea();

  // Set default text color
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int y = kHeaderHeight + 5;  // Start below header

  // Draw "Balance" label
  tft.setTextSize(2);  // Medium text
  tft.setCursor(10, y);
  tft.print("Balance");

  // Draw ADA balance in large text
  tft.setTextSize(3);  // Large text for emphasis
  y += 30;  // Move down
  tft.setCursor(10, y);
  tft.print(getWalletBalance(), 2);  // Print balance with 2 decimal places
  tft.print("ADA");  // Add "ADA" label

  // Draw stake address (smaller text)
  tft.setTextSize(1);  // Small text
  y += 35;  // Move down
  tft.setCursor(10, y);
  tft.print("Stake Address: ");
  
  // Stake addresses are long (like 57 characters), so we truncate for display
  // Show first 12 characters + "..." + last 12 characters
  // Example: "stake1u8l0y8...c5gt5k3" instead of full address
  String truncated = stakeAddress.substring(0, 12);  // First 12 chars
  truncated += "...";
  truncated += stakeAddress.substring(stakeAddress.length() - 12);  // Last 12 chars
  tft.print(truncated);

  // Display last updated time
  y += 16;  // Move down
  tft.setCursor(10, y);
  tft.print("Last updated: ");

  // Get timestamp of when balance was last fetched
  const unsigned long lastFetch = getLastKoiosFetchTime();
  
  if (lastFetch == 0) {
    // Never fetched (device just started or WiFi not connected yet)
    tft.print("Never");
  } else {
    // Calculate time difference
    const unsigned long now = millis();  // Current time
    const unsigned long diffMs = now - lastFetch;  // Difference in milliseconds
    const unsigned long diffSec = diffMs / 1000UL;  // Convert to seconds

    // Format time difference in human-readable way
    if (diffSec < 10) {
      // Less than 10 seconds ago
      tft.print("just now");
    } else if (diffSec < 60) {
      // Less than 1 minute ago - show seconds
      tft.print(diffSec);
      tft.print("s ago");
    } else {
      // 1 minute or more - show minutes and seconds
      const unsigned long minutes = diffSec / 60UL;  // Total minutes
      const unsigned long seconds = diffSec % 60UL;  // Remaining seconds
      tft.print(minutes);
      tft.print("m ");
      if (seconds > 0) {
        tft.print(seconds);
        tft.print("s ");
      }
      tft.print("ago");
    }
  }
}
