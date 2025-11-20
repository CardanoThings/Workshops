// Include TFT display library
			#include <TFT_eSPI.h>
			#include <SPI.h>

			TFT_eSPI tft = TFT_eSPI();

			// Text scrolling variables
			String scrollText = "Cardano Ticker - Real-time prices and data";
			int scrollPosition = 0;
			unsigned long lastScroll = 0;
			const unsigned long scrollDelay = 50;  // Scroll speed (milliseconds)

			void setup() {
				Serial.begin(115200);
				tft.init();
				tft.setRotation(1);
				tft.fillScreen(TFT_BLACK);
			}

			void loop() {
				unsigned long currentMillis = millis();
				
				// Scroll text periodically
				if (currentMillis - lastScroll >= scrollDelay) {
					scrollTextDisplay();
					lastScroll = currentMillis;
				}
			}

			void scrollTextDisplay() {
				// Clear screen
				tft.fillScreen(TFT_BLACK);
				
				// Calculate text position for scrolling effect
				int textWidth = scrollText.length() * 6;  // Approximate character width
				int xPos = 240 - scrollPosition;  // Start from right, scroll left
				
				// Display text at calculated position
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(xPos, 100);
				tft.println(scrollText);
				
				// Update scroll position
				scrollPosition += 2;  // Scroll speed
				
				// Reset position when text has scrolled completely off screen
				if (scrollPosition > textWidth + 240) {
					scrollPosition = 0;
				}
			}