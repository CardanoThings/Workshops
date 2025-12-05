#include "transaction_display.h"
#include <TFT_eSPI.h>
#include <qrcode_espi.h>

// External reference to the TFT display object (defined in main .ino file)
extern TFT_eSPI tft;

// QR code size in pixels (the sprite will be this size)
#define QR_SIZE 150

// Helper function to format lovelace to ADA string
String formatLovelaceToADA(unsigned long lovelace) {
  float ada = lovelace / 1000000.0;
  String adaStr = String(ada, 6);
  // Remove trailing zeros
  adaStr.trim();
  if (adaStr.indexOf('.') != -1) {
    while (adaStr.endsWith("0")) {
      adaStr.remove(adaStr.length() - 1);
    }
    if (adaStr.endsWith(".")) {
      adaStr.remove(adaStr.length() - 1);
    }
  }
  return adaStr;
}

// Function to display transaction details on the TFT screen
void displayTransaction(unsigned long amount, const char *address,
                        unsigned long timestamp) {
  // Fill entire screen with black background
  tft.fillScreen(TFT_BLACK);

  // QR code URL
  const char *qrUrl = "https://cardanothings.io";

  // ===== QR Code Sprite Setup =====
  // Create a sprite (off-screen buffer) with the specified dimensions
  TFT_eSprite qrSprite = TFT_eSprite(&tft);
  qrSprite.createSprite(QR_SIZE, QR_SIZE);

  // Fill the sprite with white background (QR codes need white background)
  qrSprite.fillSprite(TFT_WHITE);

  // ===== QR Code Generation =====
  // Create QRcode_eSPI instance using the sprite as the display
  QRcode_eSPI qrcode(&qrSprite);

  // Initialize the QR code generator with the sprite dimensions
  // This automatically calculates the scaling factor based on sprite size
  qrcode.init();

  // Generate and render the QR code to the sprite
  qrcode.create(qrUrl);

  // ===== Display Positioning =====
  // Calculate position to center the sprite horizontally on the display
  int spriteX =
      (tft.width() - QR_SIZE) / 2; // X position (centered horizontally)
  int spriteY = 10;                // Y position (top margin)

  // Push the sprite to the display at the calculated position
  qrSprite.pushSprite(spriteX, spriteY);

  // Delete the sprite to free memory
  qrSprite.deleteSprite();

  // Display payment text below QR code
  int textY = spriteY + QR_SIZE + 15;
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Format payment message
  String adaAmount = formatLovelaceToADA(amount);
  String paymentText = "Please pay " + adaAmount + " ADA to";

  // Center the text
  int textWidth = paymentText.length() * 6; // 6 pixels per char for text size 1
  int textX = (tft.width() - textWidth) / 2;
  tft.setCursor(textX, textY);
  tft.println(paymentText);

  // Display address on next line
  textY += 10;
  String addressText = String(address);
  textWidth = addressText.length() * 6;
  textX = (tft.width() - textWidth) / 2;
  tft.setCursor(textX, textY);
  tft.println(addressText);
}
