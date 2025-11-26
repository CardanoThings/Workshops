// Include TFT display library
			#include <TFT_eSPI.h>
			#include <SPI.h>

			// Create TFT display object
			TFT_eSPI tft = TFT_eSPI();

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize TFT display
				tft.init();
				
				// Set display rotation (1 = landscape)
				tft.setRotation(1);
				
				// Invert display colors (required for some CYD displays)
				tft.invertDisplay(true);
				
				// Fill entire screen with blue background
				tft.fillScreen(TFT_BLUE);
				
				// Set text properties
				tft.setTextColor(TFT_WHITE, TFT_BLUE);  // White text on blue background
				tft.setTextSize(3);                      // Large text size
				
				// Calculate text position to center it on screen
				String text = "Hello World!";
				int textWidth = text.length() * 6 * 3;   // Approximate width (6 pixels per char * text size)
				int textHeight = 8 * 3;                  // Approximate height (8 pixels * text size)
				int textX = (320 - textWidth) / 2;       // Center horizontally (320 is screen width)
				int textY = (240 - textHeight) / 2;      // Center vertically (240 is screen height)
				
				// Display the centered text
				tft.setCursor(textX, textY);
				tft.println("Hello World!");
				
				Serial.println("Display test complete!");
			}

			void loop() {
				// Nothing to do in the loop for this test
			}