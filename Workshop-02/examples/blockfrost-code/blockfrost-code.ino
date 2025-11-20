// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Blockfrost API endpoint (Preprod Testnet)
			// Note: Blockfrost uses GET requests with address in URL path
			const char* apiUrl = "https://cardano-preprod.blockfrost.io/api/v0/addresses/";
			
			// Your Blockfrost API key (get free key from blockfrost.io)
			const char* apiKey = "your-blockfrost-api-key";
			
			// Your Cardano wallet address (Preprod Testnet)
			String walletAddress = "addr_test1...";
			
			// Variables for timing balance checks
			unsigned long lastCheck = 0;                    // Timestamp of last balance check
			const unsigned long checkInterval = 30000;      // Check every 30 seconds

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				
				// Wait until WiFi is connected
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				Serial.println("Connected to WiFi");
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
					
					// Build full URL by appending wallet address to base URL
					String fullUrl = apiUrl + walletAddress;
					
					// Initialize HTTP client with full URL
					http.begin(fullUrl);
					
					// Blockfrost requires API key in "project_id" header
					http.addHeader("project_id", apiKey);
					
					// Send GET request (Blockfrost uses GET, not POST like Koios)
					int httpResponseCode = http.GET();
					
					// Check if request was successful
					if (httpResponseCode > 0) {
						// Get response body as string
						String response = http.getString();
						
						// Create JSON document to parse response (1024 bytes buffer)
						DynamicJsonDocument doc(1024);
						DeserializationError error = deserializeJson(doc, response);
						
						// Check if JSON parsing was successful
						if (!error) {
							// Blockfrost returns balance in amount array
							// Get first amount object and extract quantity
							float balance = doc["amount"][0]["quantity"] | 0.0;
							
							// Convert from Lovelace to ADA (1 ADA = 1,000,000 Lovelace)
							balance = balance / 1000000;
							
							// Print current balance
							Serial.println("Wallet Balance: " + String(balance) + " ADA");
						}
					}
					
					// Close HTTP connection
					http.end();
				}
			}