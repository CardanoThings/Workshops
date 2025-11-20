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
			
			// Your Node.js API server URL
			// Replace 'your-server-ip' with your computer's IP address on local network
			const char* apiUrl = "http://your-server-ip:3000/sensor-data";

			// Variables for timing data sends
			unsigned long lastSend = 0;                    // Timestamp of last data send
			const unsigned long sendInterval = 60000;      // Send every minute (60000 milliseconds)

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
				
				// Check if enough time has passed since last send
				if (currentMillis - lastSend >= sendInterval) {
					sendSensorData();  // Send data to API
					lastSend = currentMillis;  // Update last send timestamp
				}
			}

			void sendSensorData() {
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
					
					// Initialize HTTP client with API URL
					http.begin(apiUrl);
					
					// Set content type header for JSON request
					http.addHeader("Content-Type", "application/json");
					
					// Create JSON document to build payload
					DynamicJsonDocument doc(256);
					doc["temperature"] = t;           // Add temperature value
					doc["humidity"] = h;              // Add humidity value
					doc["timestamp"] = millis();      // Add current time in milliseconds
					
					// Serialize JSON document to string
					String jsonPayload;
					serializeJson(doc, jsonPayload);
					
					// Send POST request with JSON payload
					int httpResponseCode = http.POST(jsonPayload);
					
					// Check if request was successful
					if (httpResponseCode > 0) {
						// Get response body
						String response = http.getString();
						Serial.println("HTTP Response Code: " + String(httpResponseCode));
						Serial.println("Response: " + response);
					} else {
						Serial.println("Error in HTTP request");
					}
					
					// Close HTTP connection
					http.end();
				}
			}