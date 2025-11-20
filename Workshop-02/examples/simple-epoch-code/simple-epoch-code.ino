// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Koios API endpoint for fetching chain tip (epoch information)
			const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
			
			// Variables for timing API calls
			unsigned long lastCheck = 0;                    // Timestamp of last API call
			const unsigned long checkInterval = 60000;      // Check every minute (60000 milliseconds)
			
			// Store current epoch number
			int currentEpoch = 0;

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
				
				// Perform initial epoch data fetch on startup
				fetchEpochData();
			}

			void loop() {
				// Check if WiFi connection is still active
				if (WiFi.status() != WL_CONNECTED) {
					WiFi.reconnect();
				}
				
				// Get current time in milliseconds
				unsigned long currentMillis = millis();
				
				// Check if enough time has passed since last API call
				if (currentMillis - lastCheck >= checkInterval) {
					fetchEpochData();
					lastCheck = currentMillis;  // Update last check timestamp
				}
			}

			void fetchEpochData() {
				// Only proceed if WiFi is connected
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					
					// Initialize HTTP client with API URL
					http.begin(apiUrl);
					
					// Send GET request (tip endpoint doesn't need POST)
					int httpResponseCode = http.GET();
					
					// Check if request was successful
					if (httpResponseCode > 0) {
						// Get response body as string
						String response = http.getString();
						
						// Create JSON document to parse response
						DynamicJsonDocument doc(1024);
						DeserializationError error = deserializeJson(doc, response);
						
						// Check if JSON parsing was successful and response has data
						if (!error && doc.is<JsonArray>() && doc.size() > 0) {
							// Get first tip object from array
							JsonObject tip = doc[0];
							
							// Extract epoch number (default to 0 if not found)
							currentEpoch = tip["epoch_no"] | 0;
							
							// Print current epoch to serial monitor
							Serial.print("Current Epoch: ");
							Serial.println(currentEpoch);
						}
					}
					
					// Close HTTP connection
					http.end();
				}
			}