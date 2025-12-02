#include "asset_screen.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

namespace {
constexpr int kHeaderHeight = 34; // Must match screen_helper
constexpr int kTickerHeight = 30; // Must match ticker scroll height
} // namespace

void drawAssetScreen() {
  renderHeader("Assets", 1, 3);
  clearContentArea();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int y = kHeaderHeight + 5;
  tft.setTextSize(2);
  tft.setCursor(10, y);
  tft.print("Tokens (" + String(getTokenCount()) + ")");
  y += 35;

  const int tokenCount = getTokenCount();
  const int maxDisplay = 8; // Maximum tokens to display on screen
  const int displayCount = tokenCount < maxDisplay ? tokenCount : maxDisplay;
  tft.setTextSize(1);

  // headers
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);

  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.print("Ticker");
  tft.setCursor(60, y);
  tft.print("Amount");
  tft.setCursor(160, y);
  tft.print("Value");
  tft.setCursor(240, y);
  tft.print("24h Change");
  y += 16;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  for (int i = 0; i < displayCount; ++i) {
    TokenInfo token = getToken(i);

    // Truncate token name if too long
    String displayName = token.ticker;
    if (displayName.length() > 20) {
      displayName = displayName.substring(0, 15) + "...";
    }

    tft.setCursor(10, y);
    tft.print(displayName);

    tft.setCursor(60, y);
    tft.print(token.amount, 2);

    tft.setCursor(160, y);
    tft.print("$" + String(token.value, 2));

    tft.setCursor(220, y);
    if (token.change24h >= 0) {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
    } else {
      tft.setTextColor(TFT_RED, TFT_BLACK);
    }
    tft.print(String(token.change24h, 2));
    tft.print("%");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    y += 16;

    // Check if we've run out of space
    if (y > tft.height() - kTickerHeight - 10) {
      break;
    }
  }

  // Show count if there are more tokens
  if (tokenCount > displayCount) {
    y += 4;
    tft.setCursor(10, y);
    tft.print("... and ");
    tft.print(tokenCount - displayCount);
    tft.print(" more");
  }
}
