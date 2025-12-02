/**
 * nft_screen.cpp - NFT Collection Screen
 *
 * This screen displays all your NFT collections, showing:
 * - Collection name (e.g., "Cardano Punks", "SpaceBudz")
 * - Number of NFTs you own from that collection
 * - Floor price (lowest current selling price) in ADA
 *
 * Important Cardano NFT concepts:
 * - NFTs are grouped by "Policy ID" (collection identifier)
 * - If you own 3 NFTs from the same collection, they're shown as one entry
 * - Floor price = the cheapest NFT from that collection currently for sale
 * - Floor price helps you understand the collection's market value
 */

#include "nft_screen.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

// External reference to TFT display
extern TFT_eSPI tft;

/**
 * Draw the NFT positions screen
 *
 * This function renders a table showing all your NFT collections.
 * Each row shows one collection with its name, how many you own, and floor
 * price.
 */
void drawNFTScreen() {
  // Draw header with title and page indicator
  // activeIndex = 2 means this is the third screen (0-indexed)
  renderHeader("NFT Positions", 2);

  // Clear the content area
  clearContentArea();

  // Set default text color
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Start drawing below header
  int y = kHeaderHeight + 5;

  // Draw screen title with NFT collection count
  tft.setTextSize(2); // Larger text
  tft.setCursor(10, y);
  tft.print("NFTs(" + String(getNftCount()) + ")"); // e.g., "NFTs(3)"
  y += 35;                                          // Move down

  // Get NFT collection count (already limited to MAX_DISPLAY_ITEMS = 8)
  const int nftCount = getNftCount();
  const int displayCount = nftCount; // We can display all collections (max 8)

  // Switch to smaller text for table
  tft.setTextSize(1);

  // Draw column headers
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK); // Gray for headers

  tft.setTextSize(1);
  tft.setCursor(10, y); // "Name" column
  tft.print("Name");
  tft.setCursor(120, y); // "Amount" column
  tft.print("Amount");
  tft.setCursor(200, y); // "Floor Price" column
  tft.print("Floor Price");
  y += 16;                                // Move down to data rows
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Back to white

  // Loop through each NFT collection and draw a row
  for (int i = 0; i < displayCount; ++i) {
    // Get NFT collection data from data fetcher
    NFTInfo nft = getNFT(i);

    // Truncate collection name if too long (so it fits on screen)
    String displayName = nft.name;
    if (displayName.length() > 18) {
      // If longer than 18 characters, show first 15 + "..."
      displayName = displayName.substring(0, 15) + "...";
    }

    // Draw collection name (left column)
    tft.setCursor(10, y);
    tft.print(displayName);

    // Draw number of NFTs you own (middle column)
    tft.setCursor(120, y);
    tft.print(nft.amount, 0); // Print as integer (no decimals for count)

    // Draw floor price (right column)
    tft.setCursor(200, y);
    if (nft.floorPrice > 0.0f) {
      // If we have floor price data, show it in ADA
      tft.print(String(nft.floorPrice, 2) + " ADA"); // e.g., "50.25 ADA"
    } else {
      // If floor price not available yet (still fetching), show "N/A"
      tft.print("N/A");
    }

    // Move down for next row
    y += 16;

    // Safety check: stop if running out of screen space
    if (y > tft.height() - kTickerHeight - 10) {
      break; // Exit loop early
    }
  }

  // If there are more collections than we can display, show a message
  // (This shouldn't happen since we limit to 8, but good to have)
  if (nftCount > displayCount) {
    y += 4; // Add spacing
    tft.setCursor(10, y);
    tft.print("... and ");
    tft.print(nftCount - displayCount);
    tft.print(" more");
  }
}
