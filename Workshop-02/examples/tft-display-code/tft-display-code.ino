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
			
			// Koios API endpoint for fetching account information
			const char* apiUrl = "https://preprod.koios.rest/api/v1/account_info";
			
			// Your Cardano stake address (Preprod Testnet)
			String stakeAddress = "stake_test1...";
			
			// Variables for timing balance checks
			unsigned long lastCheck = 0;                    // Timestamp of last balance check
			const unsigned long checkInterval = 60000;      // Check every 60 seconds
			unsigned long lastFetchTime = 0;                // Timestamp of last successful fetch
			unsigned long lastDisplayUpdate = 0;            // Timestamp of last display update
			const unsigned long displayUpdateInterval = 1000; // Update display every 1 second
			
			// Store current balance to detect changes
			float currentBalance = 0.0;

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize TFT display
				tft.init();
				tft.setRotation(1);        // Set to landscape orientation
				tft.invertDisplay(true);   // Invert colors for correct display
				tft.fillScreen(TFT_BLUE);  // Fill screen with blue background
				tft.setTextColor(TFT_WHITE, TFT_BLUE);  // White text on blue background
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
				
				Serial.println("Connected to WiFi");
				Serial.print("IP address: ");
				Serial.println(WiFi.localIP());
				
				// Clear screen and show connection success
				tft.fillScreen(TFT_BLUE);
				tft.setCursor(10, 10);
				tft.println("Connected!");
				delay(1000);
				
				// Perform initial balance fetch and display
				fetchStakeBalance();
			}

			void loop() {
				// Check if WiFi connection is still active
				if (WiFi.status() != WL_CONNECTED) {
					Serial.println("WiFi connection lost. Reconnecting...");
					WiFi.reconnect();
					
					while (WiFi.status() != WL_CONNECTED) {
						delay(1000);
						Serial.print(".");
					}
					Serial.println("Reconnected!");
				}
				
				// Get current time in milliseconds
				unsigned long currentMillis = millis();
				
				// Check if enough time has passed since last check
				if (currentMillis - lastCheck >= checkInterval) {
					fetchStakeBalance();
					lastCheck = currentMillis;  // Update last check timestamp
				}
				
				// Update the timestamp display every second
				if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
					updateTimestamp();
					lastDisplayUpdate = currentMillis;
				}
			}

			void fetchStakeBalance() {
				// Only proceed if WiFi is connected
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					
					// Initialize HTTP client with API URL
					http.begin(apiUrl);
					
					// Set content type header for JSON request
					http.addHeader("Content-Type", "application/json");
					
					// Create JSON payload with stake address
					// Koios API expects stake addresses in an array under "_stake_addresses" key
					String jsonPayload = "{\\"";
					jsonPayload += "_stake_addresses";
					jsonPayload += "\\":[\\"";
					jsonPayload += stakeAddress;
					jsonPayload += "\\"]}";
					
					// Send POST request and get response code
					int httpResponseCode = http.POST(jsonPayload);
					
					// Check if request was successful (response code > 0)
					if (httpResponseCode > 0) {
						// Get response body as string
						String response = http.getString();
						Serial.println("HTTP Response Code: " + String(httpResponseCode));
						
						// Create JSON document to parse response (2048 bytes buffer)
						DynamicJsonDocument doc(2048);
						DeserializationError error = deserializeJson(doc, response);
						
						// Check if JSON parsing was successful
						if (!error) {
							// Verify response is an array with at least one element
							if (doc.is<JsonArray>() && doc.size() > 0) {
								// Get first account info object from array
								JsonObject accountInfo = doc[0];
								
								// Extract total balance as string (Koios returns balance as string)
								// total_balance includes delegated amount + rewards
								const char* balanceStr = accountInfo["total_balance"];
								
								// Convert string to long long (for large Lovelace values)
								long long balanceLovelace = 0;
								if (balanceStr != nullptr) {
									balanceLovelace = atoll(balanceStr);
								}
								
								// Convert from Lovelace to tADA (test ADA)
								// 1 tADA = 1,000,000 Lovelace
								float balance = balanceLovelace / 1000000.0;
								
								// Print account information
								Serial.println("Stake Address: " + String(accountInfo["stake_address"].as<const char*>()));
								Serial.println("Total Balance: " + String(balance, 6) + " tADA");
								
								// Check if balance has changed since last check
								if (balance != currentBalance) {
									if (balance > currentBalance) {
										Serial.println("✓ Balance increased!");
									} else if (balance < currentBalance) {
										Serial.println("✓ Balance decreased!");
									}
									// Update current balance
									currentBalance = balance;
								}
								
								// Update last fetch time and refresh full display
								lastFetchTime = millis();
								updateDisplay();
							}
						} else {
							// Print error if JSON parsing failed
							Serial.print("JSON parsing failed: ");
							Serial.println(error.c_str());
						}
					} else {
						// Print error if HTTP request failed
						Serial.println("Error in HTTP request");
						Serial.println("HTTP Response Code: " + String(httpResponseCode));
					}
					
					// Close HTTP connection
					http.end();
				} else {
					Serial.println("WiFi not connected");
				}
			}

			void updateDisplay() {
				// Fill entire screen with blue background
				tft.fillScreen(TFT_BLUE);
				
				// Display title "Wallet Balance" in white
				tft.setTextSize(2);
				tft.setTextColor(TFT_WHITE, TFT_BLUE);
				tft.setCursor(10, 10);
				tft.println("Wallet Balance");
				
				// Display balance amount in large white text
				tft.setTextSize(4);
				tft.setTextColor(TFT_WHITE, TFT_BLUE);
				tft.setCursor(10, 50);
				tft.print(String(currentBalance, 2));  // Format to 2 decimal places
				
				// Display "ADA" unit
				tft.setTextSize(2);
				tft.println(" ADA");
				
				// Display initial timestamp
				updateTimestamp();
			}
			
			void updateTimestamp() {
				// Calculate seconds since last fetch
				unsigned long secondsAgo = (millis() - lastFetchTime) / 1000;
				
				// Clear the timestamp area (blue rectangle over the old text)
				tft.fillRect(0, 225, 200, 10, TFT_BLUE);
				
				// Display last update timestamp in lower left corner
				tft.setTextSize(1);
				tft.setTextColor(TFT_WHITE, TFT_BLUE);
				tft.setCursor(5, 225);
				tft.print("Updated ");
				tft.print(secondsAgo);
				tft.println("s ago");
			}