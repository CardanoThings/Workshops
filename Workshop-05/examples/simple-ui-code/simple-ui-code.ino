// Include TFT display library
			#include <TFT_eSPI.h>
			#include <SPI.h>

			TFT_eSPI tft = TFT_eSPI();

			// Simple button structure
			struct SimpleButton {
				int x, y, width, height;
				String text;
			};

			// Create a button
			SimpleButton myButton = {50, 100, 140, 40, "Pay Now"};

			void setup() {
				Serial.begin(115200);
				tft.init();
				tft.setRotation(1);
				tft.fillScreen(TFT_BLACK);
				
				// Draw button
				drawButton(myButton);
			}

			void loop() {
				// Check for touch
				uint16_t x, y;
				if (tft.getTouch(&x, &y)) {
					// Check if touch is on button
					if (x >= myButton.x && x <= myButton.x + myButton.width &&
					    y >= myButton.y && y <= myButton.y + myButton.height) {
						// Button pressed
						handleButtonPress();
						delay(300);  // Debounce
					}
				}
			}

			void drawButton(SimpleButton btn) {
				// Draw button background
				tft.fillRect(btn.x, btn.y, btn.width, btn.height, TFT_BLUE);
				tft.drawRect(btn.x, btn.y, btn.width, btn.height, TFT_WHITE);
				
				// Draw button text
				tft.setTextSize(2);
				tft.setTextColor(TFT_WHITE, TFT_BLUE);
				tft.setCursor(btn.x + 20, btn.y + 10);
				tft.println(btn.text);
			}

			void handleButtonPress() {
				// Handle button press action
				tft.fillScreen(TFT_BLACK);
				tft.setTextSize(2);
				tft.setTextColor(TFT_GREEN, TFT_BLACK);
				tft.setCursor(10, 50);
				tft.println("Button Pressed!");
			}