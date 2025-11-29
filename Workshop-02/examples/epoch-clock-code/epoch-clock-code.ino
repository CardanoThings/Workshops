// Include necessary libraries
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>
			#include <Adafruit_NeoPixel.h>
			#include <WiFiClientSecure.h>

			// Pin connected to the WS2812 data input
			#define LED_PIN 4
			
			// Number of LEDs in the ring (12 LEDs)
			#define NUM_LEDS 12
			
			// Total slots in an epoch (approximately 432,000 on Mainnet)
			#define SLOTS_PER_EPOCH 432000
			
			// Create NeoPixel object
			Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

			// WiFi credentials
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// Koios API endpoint
			const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
			
			// Variables for timing API calls
			unsigned long lastCheck = 0;
			const unsigned long checkInterval = 60000;  // Check every minute
			
			// Variables for walking LED (5 seconds per LED, 12 LEDs = 1 minute)
			unsigned long lastWalkUpdate = 0;
			const unsigned long walkFadeInterval = 50;  // Update every 50ms for smooth fade
			const unsigned long walkDuration = 5000;  // 5 seconds per LED
			int walkPosition = 0;  // Current position of walking LED (0-11)
			int walkBrightness = 0;  // Current brightness (0-255)
			int walkFadeDirection = 1;  // 1 = fading in, -1 = fading out
			unsigned long walkStartTime = 0;  // When current LED started
			
			// Store current epoch data
			int currentEpoch = 0;
			int currentEpochSlot = 0;
			int lastEpoch = -1;

			void setup() {
				Serial.begin(115200);
				
				// Initialize LED ring
				strip.begin();
				strip.setBrightness(5);  // Low brightness for safety
				strip.clear();
				strip.show();
				
				// Connect to WiFi
				WiFi.begin(ssid, password);
				WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Workaround for ESP32-C3 Super Mini
				
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
				}
				
				// Initial fetch
				fetchEpochData();
				displayProgress();
				
				// Initialize walking LED timing
				walkStartTime = millis();
			}

			void loop() {
				// Check WiFi connection
				if (WiFi.status() != WL_CONNECTED) {
					WiFi.reconnect();
					while (WiFi.status() != WL_CONNECTED) {
						delay(1000);
					}
				}
				
				// Check if enough time has passed for API call
				unsigned long currentMillis = millis();
				if (currentMillis - lastCheck >= checkInterval) {
					fetchEpochData();
					displayProgress();
					lastCheck = currentMillis;
				}
				
				// Update walking LED fade effect
				if (currentMillis - lastWalkUpdate >= walkFadeInterval) {
					updateWalkingLED();
					lastWalkUpdate = currentMillis;
				}
			}

			void fetchEpochData() {
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					WiFiClientSecure client;
					
					client.setInsecure();
					http.begin(client, apiUrl);
					
					int httpResponseCode = http.GET();
					
					if (httpResponseCode > 0) {
						String response = http.getString();
						
						JsonDocument doc;
						DeserializationError error = deserializeJson(doc, response);
						
						if (!error && doc.is<JsonArray>() && doc.size() > 0) {
							JsonObject tip = doc[0];
							currentEpoch = tip["epoch_no"] | 0;
							currentEpochSlot = tip["epoch_slot"] | 0;
							
							// Reset display if epoch changed
							if (currentEpoch != lastEpoch) {
								lastEpoch = currentEpoch;
								strip.clear();
								strip.show();
								delay(500);
							}
						}
					}
					
					http.end();
				}
			}

			void displayProgress() {
				// Calculate epoch progress percentage
				int progressPercent = (currentEpochSlot * 100) / SLOTS_PER_EPOCH;
				if (progressPercent > 100) progressPercent = 100;
				
				// Calculate how many LEDs should be lit
				int ledsToLight = (progressPercent * NUM_LEDS) / 100;
				
				// Clear all LEDs
				strip.clear();
				
				// Light up LEDs based on progress in blue
				for (int i = 0; i < ledsToLight; i++) {
					strip.setPixelColor(i, strip.Color(0, 0, 255));  // Blue
				}
				
				strip.show();
			}

			void updateWalkingLED() {
				unsigned long currentMillis = millis();
				
				// Check if we need to move to next LED (after 5 seconds)
				if (currentMillis - walkStartTime >= walkDuration) {
					walkPosition = (walkPosition + 1) % NUM_LEDS;
					walkStartTime = currentMillis;
					walkBrightness = 0;
					walkFadeDirection = 1;  // Start fading in
				}
				
				// Calculate elapsed time for current LED (0 to 5000ms)
				unsigned long elapsed = currentMillis - walkStartTime;
				
				// Fade in for first 2.5 seconds, fade out for next 2.5 seconds
				if (elapsed < 2500) {
					// Fade in: 0 to 255 over 2.5 seconds
					walkBrightness = (elapsed * 255) / 2500;
				} else {
					// Fade out: 255 to 0 over 2.5 seconds
					walkBrightness = 255 - ((elapsed - 2500) * 255) / 2500;
				}
				
				// Display epoch progress first (blue LEDs)
				displayProgress();
				
				// Add white walking LED with current brightness
				strip.setPixelColor(walkPosition, strip.Color(walkBrightness, walkBrightness, walkBrightness));
				strip.show();
			}