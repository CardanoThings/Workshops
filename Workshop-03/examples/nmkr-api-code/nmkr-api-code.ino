// Include necessary libraries
			#include <WiFi.h>              // WiFi connectivity
			#include <HTTPClient.h>       // HTTP client for API calls
			#include <ArduinoJson.h>      // JSON parsing and creation
			#include <DHT.h>               // DHT sensor library

			// DHT sensor pin configuration
			#define DHTPIN 4               // Digital GPIO pin connected to DHT22 data pin
			#define DHTTYPE DHT22          // Sensor type: DHT22

			// Create DHT sensor object
			DHT dht(DHTPIN, DHTTYPE);

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// NMKR API endpoint for minting NFTs
			const char* nmkrApiUrl = "https://api.nmkr.io/v2/mint";
			
			// Your NMKR API key (get from nmkr.io dashboard)
			const char* nmkrApiKey = "your-nmkr-api-key";

			// Variables for timing NFT mints
			unsigned long lastMint = 0;                    // Timestamp of last mint
			const unsigned long mintInterval = 300000;     // Mint every 5 minutes (300000 milliseconds)

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize DHT sensor
				dht.begin();
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
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
				
				// Check if enough time has passed since last mint
				if (currentMillis - lastMint >= mintInterval) {
					mintNFT();  // Mint NFT with current sensor data
					lastMint = currentMillis;  // Update last mint timestamp
				}
			}

			void mintNFT() {
				// Read humidity from DHT sensor
				float h = dht.readHumidity();
				
				// Read temperature from DHT sensor
				float t = dht.readTemperature();
				
				// Check if sensor reads failed
				if (isnan(h) || isnan(t)) {
					Serial.println("Failed to read from DHT sensor!");
					return;  // Exit function if read failed
				}
				
				// Only proceed if WiFi is connected
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					
					// Initialize HTTP client with NMKR API URL
					http.begin(nmkrApiUrl);
					
					// Set content type header for JSON request
					http.addHeader("Content-Type", "application/json");
					
					// Set NMKR API key in header (required for authentication)
					http.addHeader("X-API-KEY", nmkrApiKey);
					
					// Create JSON document to build mint request payload
					DynamicJsonDocument doc(1024);
					
					// Set project ID (get from NMKR dashboard)
					doc["projectId"] = "your-project-id";
					
					// Set wallet address to receive the minted NFT
					doc["receiverAddress"] = "addr_test1...";  // Your Cardano wallet address
					
					// Create nested metadata object with sensor data
					JsonObject metadata = doc.createNestedObject("metadata");
					metadata["temperature"] = String(t, 2);    // Temperature with 2 decimal places
					metadata["humidity"] = String(h, 2);       // Humidity with 2 decimal places
					metadata["timestamp"] = String(millis());  // Current time in milliseconds
					metadata["name"] = "Sensor Data NFT";       // NFT name
					metadata["description"] = "NFT minted with temperature and humidity data";  // NFT description
					
					// Serialize JSON document to string
					String jsonPayload;
					serializeJson(doc, jsonPayload);
					
					// Send POST request to NMKR API
					int httpResponseCode = http.POST(jsonPayload);
					
					// Check if request was successful
					if (httpResponseCode > 0) {
						// Get response body
						String response = http.getString();
						Serial.println("HTTP Response Code: " + String(httpResponseCode));
						Serial.println("Response: " + response);
					} else {
						// Print error if request failed
						Serial.println("Error in HTTP request");
						Serial.println("HTTP Response Code: " + String(httpResponseCode));
					}
					
					// Close HTTP connection
					http.end();
				}
			}