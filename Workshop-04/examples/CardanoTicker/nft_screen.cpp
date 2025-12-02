#include "nft_screen.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

namespace {
constexpr int kHeaderHeight = 34; // Must match screen_helper
constexpr int kTickerHeight = 30; // Must match ticker scroll height
} // namespace

void drawNFTScreen() {
  renderHeader("NFT Positions", 2);
  clearContentArea();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int y = kHeaderHeight + 5;
  tft.setTextSize(2);
  tft.setCursor(10, y);
  tft.print("NFTs(" + String(getNftCount()) + ")");
  y += 35;

  const int nftCount = getNftCount();
  const int maxDisplay = 8; // Maximum NFTs to display on screen
  const int displayCount = nftCount < maxDisplay ? nftCount : maxDisplay;
  tft.setTextSize(1);

  // headers
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);

  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.print("Name");
  tft.setCursor(120, y);
  tft.print("Amount");
  tft.setCursor(200, y);
  tft.print("Floor Price");
  y += 16;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  for (int i = 0; i < displayCount; ++i) {
    NFTInfo nft = getNFT(i);

    // Truncate NFT name if too long
    String displayName = nft.name;
    if (displayName.length() > 18) {
      displayName = displayName.substring(0, 15) + "...";
    }

    tft.setCursor(10, y);
    tft.print(displayName);

    tft.setCursor(120, y);
    tft.print(nft.amount, 0);

    tft.setCursor(200, y);
    if (nft.floorPrice > 0.0f) {
      tft.print(String(nft.floorPrice, 2) + " ADA");
    } else {
      tft.print("N/A");
    }

    y += 16;

    // Check if we've run out of space
    if (y > tft.height() - kTickerHeight - 10) {
      break;
    }
  }

  // Show count if there are more NFTs
  if (nftCount > displayCount) {
    y += 4;
    tft.setCursor(10, y);
    tft.print("... and ");
    tft.print(nftCount - displayCount);
    tft.print(" more");
  }
}
