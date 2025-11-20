// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Koios API endpoint for fetching address information
			const char* apiUrl = "https://preprod.koios.rest/api/v1/address_info";
			
			// Your Cardano wallet address (Preprod Testnet)
			String walletAddress = "addr_test1...";
			
			// Variables for timing balance checks
			unsigned long lastCheck = 0;                    // Timestamp of last balance check
			const unsigned long checkInterval = 30000;      // Check every 30 seconds (30000 milliseconds)
			
			// Store previous balance to detect changes
			float previousBalance = 0;

			void setup() {
				// Initialize serial communication for debugging (115200 baud rate)
				Serial.begin(115200);
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				
				// Wait until WiFi is connected
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				
				// Print connection confirmation and IP address
				Serial.println("Connected to WiFi");
				Serial.print("IP address: ");
				Serial.println(WiFi.localIP());
				
				// Perform initial balance check on startup
				fetchWalletBalance();
			}

			void loop() {
				// Check if WiFi connection is still active
				if (WiFi.status() != WL_CONNECTED) {
					Serial.println("WiFi connection lost. Reconnecting...");
					WiFi.reconnect();
					
					// Wait for reconnection
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
					// Koios API expects addresses in an array under "_addresses" key
					String jsonPayload = "{\"_addresses\":[\"" + walletAddress + "\"]}";
					
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
								// Get first address info object from array
								JsonObject addressInfo = doc[0];
								
								// Extract balance (default to 0.0 if not found)
								float balance = addressInfo["balance"] | 0.0;
								
								// Convert from Lovelace (smallest unit) to ADA
								// 1 ADA = 1,000,000 Lovelace
								balance = balance / 1000000;
								
								// Print current balance
								Serial.println("Wallet Balance: " + String(balance) + " ADA");
								
								// Check if balance has changed since last check
								if (balance != previousBalance) {
									if (balance > previousBalance) {
										Serial.println("Balance increased!");
									} else {
										Serial.println("Balance decreased!");
									}
									// Update previous balance for next comparison
									previousBalance = balance;
								}
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