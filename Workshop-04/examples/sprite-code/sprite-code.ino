// Include TFT display library
			#include <TFT_eSPI.h>
			#include <SPI.h>

			TFT_eSPI tft = TFT_eSPI();

			// Create sprite for smooth animations
			// Sprite is an off-screen buffer that can be drawn to screen
			TFT_eSprite sprite = TFT_eSprite(&tft);

			void setup() {
				Serial.begin(115200);
				tft.init();
				tft.setRotation(1);
				tft.fillScreen(TFT_BLACK);
				
				// Create sprite with specified dimensions
				sprite.createSprite(240, 135);  // Width, Height
			}

			void loop() {
				// Draw to sprite (off-screen buffer)
				drawToSprite();
				
				// Push sprite to display (shows the sprite on screen)
				sprite.pushSprite(0, 0);
				
				delay(100);
			}

			void drawToSprite() {
				// Clear sprite with black background
				sprite.fillScreen(TFT_BLACK);
				
				// Draw title
				sprite.setTextSize(2);
				sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
				sprite.setCursor(10, 10);
				sprite.println("Cardano Ticker");
				
				// Draw a line
				sprite.drawLine(10, 35, 230, 35, TFT_WHITE);
				
				// Draw token information
				sprite.setTextSize(2);
				sprite.setTextColor(TFT_CYAN, TFT_BLACK);
				sprite.setCursor(10, 50);
				sprite.print("ADA: $");
				sprite.setTextColor(TFT_GREEN, TFT_BLACK);
				sprite.println("0.45");
				
				// Draw rectangle around important data
				sprite.drawRect(5, 45, 235, 40, TFT_BLUE);
				
				// Draw filled circle as indicator
				sprite.fillCircle(220, 65, 5, TFT_GREEN);
			}