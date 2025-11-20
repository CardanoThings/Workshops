// Include necessary libraries
			#include <WiFi.h>              // WiFi connectivity
			#include <HTTPClient.h>       // HTTP client for API calls
			#include <ArduinoJson.h>      // JSON parsing
			#include <TFT_eSPI.h>         // TFT display library
			#include <SPI.h>               // SPI communication for display

			// Create TFT display object
			TFT_eSPI tft = TFT_eSPI();

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Koios API endpoint for fetching address information
			const char* apiUrl = "https://preprod.koios.rest/api/v1/address_info";
			
			// Your Cardano wallet address (Preprod Testnet)
			String walletAddress = "addr_test1...";
			
			// Variables for timing balance checks
			unsigned long lastCheck = 0;                    // Timestamp of last balance check
			const unsigned long checkInterval = 30000;      // Check every 30 seconds
			
			// Store current balance to detect changes
			float currentBalance = 0.0;

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize TFT display
				tft.init();
				tft.setRotation(1);        // Set to landscape orientation (can be 0-3)
				tft.fillScreen(TFT_BLACK);  // Clear screen with black background
				tft.setTextColor(TFT_WHITE, TFT_BLACK);  // White text on black background
				tft.setTextSize(2);        // Set text size
				
				// Display startup message on screen
				tft.setCursor(10, 10);
				tft.println("Connecting...");
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				
				// Clear screen and show connection success
				tft.fillScreen(TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Connected!");
				delay(1000);
				
				// Perform initial balance fetch and display
				fetchWalletBalance();
			}

			void loop() {
				// Check if WiFi connection is still active
				if (WiFi.status() != WL_CONNECTED) {
					WiFi.reconnect();
					while (WiFi.status() != WL_CONNECTED) {
						delay(1000);
					}
				}
				
				// Get current time in milliseconds
				unsigned long currentMillis = millis();
				
				// Check if enough time has passed since last check
				if (currentMillis - lastCheck >= checkInterval) {
					fetchWalletBalance();
					lastCheck = currentMillis;  // Update last check timestamp
				}
			}

			void fetchWalletBalance() {
				// Only proceed if WiFi is connected
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					
					// Initialize HTTP client with API URL
					http.begin(apiUrl);
					
					// Set content type header for JSON request
					http.addHeader("Content-Type", "application/json");
					
					// Create JSON payload with wallet address
					String jsonPayload = "{\"_addresses\":[\"" + walletAddress + "\"]}";
					
					// Send POST request
					int httpResponseCode = http.POST(jsonPayload);
					
					// Check if request was successful
					if (httpResponseCode > 0) {
						// Get response body as string
						String response = http.getString();
						
						// Create JSON document to parse response
						DynamicJsonDocument doc(2048);
						DeserializationError error = deserializeJson(doc, response);
						
						// Check if JSON parsing was successful and response has data
						if (!error && doc.is<JsonArray>() && doc.size() > 0) {
							// Get first address info object from array
							JsonObject addressInfo = doc[0];
							
							// Extract balance and convert from Lovelace to ADA
							float balance = addressInfo["balance"] | 0.0;
							balance = balance / 1000000;  // 1 ADA = 1,000,000 Lovelace
							
							// Only update display if balance changed (saves screen updates)
							if (balance != currentBalance) {
								currentBalance = balance;
								updateDisplay();  // Refresh display with new balance
							}
						}
					}
					
					// Close HTTP connection
					http.end();
				}
			}

			void updateDisplay() {
				// Clear entire screen with black background
				tft.fillScreen(TFT_BLACK);
				
				// Display title "Wallet Balance" in cyan
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Wallet Balance");
				
				// Display balance amount in large green text
				tft.setTextSize(4);                    // Large text for balance
				tft.setTextColor(TFT_GREEN, TFT_BLACK);
				tft.setCursor(10, 50);
				tft.print(String(currentBalance, 2));  // Format to 2 decimal places
				
				// Display "ADA" unit in smaller text
				tft.setTextSize(2);
				tft.println(" ADA");
				
				// Display last update timestamp in small gray text
				tft.setTextSize(1);
				tft.setTextColor(TFT_GRAY, TFT_BLACK);
				tft.setCursor(10, 120);
				tft.print("Last update: ");
				tft.print(millis() / 1000);  // Convert milliseconds to seconds
				tft.println("s ago");
			}