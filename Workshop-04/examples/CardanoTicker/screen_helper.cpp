#include "screen_helper.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

namespace {
constexpr int kHeaderHeight = 34;
constexpr int kTickerHeight = 30; // Must match ticker scroll height
constexpr int kIndicatorRadius = 3;
constexpr int kIndicatorSpacing = 3;
constexpr int kIndicatorMargin = 5;

TFT_eSprite headerSprite = TFT_eSprite(&tft);
bool headerSpriteInitialized = false;

void ensureHeaderSprite() {
  if (!headerSpriteInitialized || headerSprite.width() != tft.width()) {
    if (headerSpriteInitialized) {
      headerSprite.deleteSprite();
    }
    headerSprite.setColorDepth(16);
    headerSprite.createSprite(tft.width(), kHeaderHeight);
    headerSpriteInitialized = true;
  }
}
} // namespace

void renderHeader(const char *title, uint8_t activeIndex,
                  uint8_t totalScreens) {
  ensureHeaderSprite();

  headerSprite.fillSprite(TFT_BLACK);
  headerSprite.setTextColor(TFT_WHITE, TFT_BLACK);
  headerSprite.setTextSize(1);
  headerSprite.setCursor(5, 6);
  if (title != nullptr) {
    headerSprite.print(title);
  }
  headerSprite.drawFastHLine(5, 20, headerSprite.width() - 10, TFT_WHITE);

  if (totalScreens > 0) {
    const int totalWidth =
        totalScreens * (kIndicatorRadius * 2) +
        (static_cast<int>(totalScreens) - 1) * kIndicatorSpacing;
    int startX = headerSprite.width() - kIndicatorMargin - totalWidth;
    if (startX < kIndicatorMargin) {
      startX = kIndicatorMargin;
    }
    const int centerY = 10;

    for (uint8_t i = 0; i < totalScreens; ++i) {
      const int cx = startX + kIndicatorRadius +
                     i * ((kIndicatorRadius * 2) + kIndicatorSpacing);
      if (i == activeIndex) {
        headerSprite.fillCircle(cx, centerY, kIndicatorRadius, TFT_WHITE);
      } else {
        headerSprite.fillCircle(cx, centerY, kIndicatorRadius, TFT_BLACK);
      }
      headerSprite.drawCircle(cx, centerY, kIndicatorRadius, TFT_WHITE);
    }
  }

  headerSprite.pushSprite(0, 0);
}

void clearContentArea() {
  const int top = kHeaderHeight;
  const int height = tft.height() - kHeaderHeight - kTickerHeight;
  if (height <= 0) {
    return;
  }
  tft.fillRect(0, top, tft.width(), height, TFT_BLACK);
}
