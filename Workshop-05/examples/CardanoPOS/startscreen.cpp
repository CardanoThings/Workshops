#include "startscreen.h"
#include <TFT_eSPI.h>

// External reference to the TFT display object (defined in main .ino file)
extern TFT_eSPI tft;

// Function to display the start screen
void displayStartScreen() {
  // Fill entire screen with black background
  tft.fillScreen(TFT_BLACK);

  // Set text properties for main title
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // White text on black background
  tft.setTextSize(3);                     // Large text size

  // Calculate main title position to center it on screen
  String mainText = "CardanoPOS";
  int mainTextWidth = mainText.length() * 6 *
                      3; // Approximate width (6 pixels per char * text size)
  int mainTextHeight = 8 * 3; // Approximate height (8 pixels * text size)
  int mainTextX = (tft.width() - mainTextWidth) /
                  2; // Center horizontally (320 is screen width)
  int mainTextY = (tft.height() - mainTextHeight) / 2 -
                  15; // Center vertically, slightly above center

  // Display the centered main title
  tft.setCursor(mainTextX, mainTextY);
  tft.println(mainText);

  // Set text properties for website URL (smaller text)
  tft.setTextSize(1); // Smaller text size

  // Calculate website URL position to center it below the main title
  String urlText = "www.cardanothings.io";
  int urlTextWidth = urlText.length() * 6 *
                     1; // Approximate width (6 pixels per char * text size)
  int urlTextX = (tft.width() - urlTextWidth) / 2; // Center horizontally
  int urlTextY =
      mainTextY + mainTextHeight + 10; // Position below main title with spacing

  // Display the centered website URL
  tft.setCursor(urlTextX, urlTextY);
  tft.println("www.cardanothings.io");
}
