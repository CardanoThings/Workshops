// Include necessary libraries
			#include <WiFi.h>              // WiFi connectivity
			#include <HTTPClient.h>       // HTTP client for API calls
			#include <ArduinoJson.h>      // JSON parsing
			#include <MD_Parola.h>         // LED matrix text display library
			#include <MD_MAX72XX.h>       // LED matrix hardware control
			#include <SPI.h>               // SPI communication for LED matrix

			// LED Matrix hardware configuration
			#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // Hardware type (FC16 is common)
			#define MAX_DEVICES 4                      // Number of 8x8 matrix modules (4 = 32x8 display)
			#define CS_PIN 5                            // Chip Select pin
			#define DATA_PIN 23                         // Data pin (MOSI)
			#define CLK_PIN 18                          // Clock pin (SCK)

			// Create display object with hardware configuration
			MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Koios API endpoint for fetching chain tip (epoch information)
			const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
			
			// Variables for timing API calls
			unsigned long lastCheck = 0;                    // Timestamp of last API call
			const unsigned long checkInterval = 60000;      // Check every minute (60000 milliseconds)
			
			// Store current epoch data
			int currentEpoch = 0;        // Current epoch number
			int currentEpochSlot = 0;    // Current slot within epoch

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize LED Matrix display
				myDisplay.begin();
				myDisplay.setIntensity(5);              // Set brightness (0-15)
				myDisplay.displayClear();                // Clear display
				
				// Display initialization message
				myDisplay.displayText("INIT", PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				Serial.println("Connected to WiFi");
				
				// Perform initial epoch data fetch
				fetchEpochData();
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
				
				// Check if enough time has passed since last API call
				if (currentMillis - lastCheck >= checkInterval) {
					fetchEpochData();
					lastCheck = currentMillis;  // Update last check timestamp
				}
				
				// Update display animation (required for MD_Parola library)
				// Returns true when animation is complete
				if (myDisplay.displayAnimate()) {
					displayEpoch();  // Update display with new data
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
							
							// Extract epoch number and slot (default to 0 if not found)
							currentEpoch = tip["epoch_no"] | 0;
							currentEpochSlot = tip["epoch_slot"] | 0;
							
							// Print to serial monitor for debugging
							Serial.print("Epoch: ");
							Serial.print(currentEpoch);
							Serial.print(" Slot: ");
							Serial.println(currentEpochSlot);
						}
					}
					
					// Close HTTP connection
					http.end();
				}
			}

			void displayEpoch() {
				// Create text string for epoch number (e.g., "E252")
				String epochText = "E" + String(currentEpoch);
				
				// Display epoch text centered with no animation effects
				myDisplay.displayText(epochText.c_str(), PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);
				
				// Wait 2 seconds before showing slot
				delay(2000);
				
				// Create text string for epoch slot (e.g., "S12345")
				String slotText = "S" + String(currentEpochSlot);
				
				// Display slot text centered with no animation effects
				myDisplay.displayText(slotText.c_str(), PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);
			}