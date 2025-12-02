#include "wallet_screen.h"
#include "config.h"
#include "data_fetcher.h"
#include "screen_helper.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

namespace {
constexpr int kHeaderHeight = 34; // Must match screen_helper
} // namespace

void drawWalletScreen() {
  renderHeader("Wallet", 0, 3);
  clearContentArea();

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int y = kHeaderHeight + 5;

  tft.setTextSize(2);
  tft.setCursor(10, y);
  tft.print("Balance");

  tft.setTextSize(3);
  y += 30;
  tft.setCursor(10, y);
  tft.print(getWalletBalance(), 2);
  tft.print("ADA");

  tft.setTextSize(1);
  y += 35;
  tft.setCursor(10, y);
  tft.print("Stake Address: ");
  // Always display first 12 and last 12 characters of stake address on same
  // line
  String truncated = stakeAddress.substring(0, 12);
  truncated += "...";
  truncated += stakeAddress.substring(stakeAddress.length() - 12);
  tft.print(truncated);

  // Display last updated time
  y += 16;
  tft.setCursor(10, y);
  tft.print("Last updated: ");

  const unsigned long lastFetch = getLastKoiosFetchTime();
  if (lastFetch == 0) {
    tft.print("Never");
  } else {
    const unsigned long now = millis();
    const unsigned long diffMs = now - lastFetch;
    const unsigned long diffSec = diffMs / 1000UL;

    if (diffSec < 10) {
      tft.print("just now");
    } else if (diffSec < 60) {
      tft.print(diffSec);
      tft.print("s ago");
    } else {
      const unsigned long minutes = diffSec / 60UL;
      const unsigned long seconds = diffSec % 60UL;
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
