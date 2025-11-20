// Include TFT display library and SPI communication
			#include <TFT_eSPI.h>
			#include <SPI.h>

			// Create TFT display object
			TFT_eSPI tft = TFT_eSPI();

			void setup() {
				// Initialize TFT display
				tft.init();
				tft.setRotation(1);        // Set to landscape orientation
				tft.fillScreen(TFT_BLACK);  // Clear screen with black background
				
				// Display formatted text immediately
				displayFormattedData();
			}

			void loop() {
				// Your main loop - add your code here
				// This example just displays static data
			}

			void displayFormattedData() {
				// Clear entire screen with black background
				tft.fillScreen(TFT_BLACK);
				
				// Display title "Cardano Data" in large yellow text
				tft.setTextSize(3);                    // Large text size
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);  // Yellow text on black
				tft.setCursor(10, 10);                // Position cursor
				tft.println("Cardano Data");
				
				// Draw a horizontal line separator (from x=10 to x=230 at y=45)
				tft.drawLine(10, 45, 230, 45, TFT_WHITE);
				
				// Display "Balance: " label in medium cyan text
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(10, 60);
				tft.print("Balance: ");
				
				// Display balance value in large green text
				tft.setTextSize(3);
				tft.setTextColor(TFT_GREEN, TFT_BLACK);
				tft.print("100.50");
				
				// Display "ADA" unit in medium cyan text
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.println(" ADA");
				
				// Draw a rectangle border around the balance data
				// Parameters: x, y, width, height, color
				tft.drawRect(5, 55, 235, 50, TFT_BLUE);
			}