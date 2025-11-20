// Include necessary libraries
			#include <WiFi.h>              // WiFi connectivity (optional for this example)
			#include <DHT.h>               // DHT sensor library by Adafruit
			#include <TFT_eSPI.h>          // TFT display library
			#include <SPI.h>               // SPI communication for display

			// DHT sensor pin configuration
			#define DHTPIN 4               // Digital GPIO pin connected to DHT22 data pin
			#define DHTTYPE DHT22          // Sensor type: DHT22 (also known as AM2302)

			// Create sensor and display objects
			DHT dht(DHTPIN, DHTTYPE);     // Initialize DHT sensor on specified pin
			TFT_eSPI tft = TFT_eSPI();    // Create TFT display object

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";

			// Variables to store sensor readings
			float temperature = 0;        // Current temperature reading
			float humidity = 0;           // Current humidity reading
			
			// Variables for timing sensor reads
			unsigned long lastRead = 0;                    // Timestamp of last sensor read
			const unsigned long readInterval = 2000;       // Read every 2 seconds (DHT22 minimum)

			void setup() {
				// Initialize serial communication for debugging (115200 baud rate)
				Serial.begin(115200);
				
				// Initialize DHT sensor
				dht.begin();
				
				// Initialize TFT display
				tft.init();                        // Initialize display hardware
				tft.setRotation(1);                // Set to landscape orientation
				tft.fillScreen(TFT_BLACK);         // Clear screen with black background
				tft.setTextColor(TFT_WHITE, TFT_BLACK);  // White text on black background
				tft.setTextSize(2);                // Set default text size
				
				// Display startup message on screen
				tft.setCursor(10, 10);
				tft.println("Initializing...");
				
				// Start WiFi connection (optional - can be removed if not needed)
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				
				Serial.println("Connected to WiFi");
				
				// Clear screen and show ready message
				tft.fillScreen(TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Ready!");
				delay(1000);
			}

			void loop() {
				// Get current time in milliseconds
				unsigned long currentMillis = millis();
				
				// Check if enough time has passed since last sensor read
				// DHT22 requires at least 2 seconds between readings
				if (currentMillis - lastRead >= readInterval) {
					readSensorData();  // Read from sensor
					displayData();     // Update display
					lastRead = currentMillis;  // Update last read timestamp
				}
			}

			void readSensorData() {
				// Read humidity from sensor (takes ~250ms)
				// Note: Sensor readings may be up to 2 seconds old
				float h = dht.readHumidity();
				
				// Read temperature from sensor (takes ~250ms)
				float t = dht.readTemperature();
				
				// Check if any reads failed (returns NaN on failure)
				if (isnan(h) || isnan(t)) {
					Serial.println("Failed to read from DHT sensor!");
					return;  // Exit function if read failed
				}
				
				// Compute heat index (feels-like temperature)
				// Parameters: temperature, humidity, isFahrenheit (false = Celsius)
				float hi = dht.computeHeatIndex(t, h, false);
				
				// Store readings in global variables
				temperature = t;
				humidity = h;
				
				// Print readings to serial monitor for debugging
				Serial.print("Humidity: ");
				Serial.print(humidity);
				Serial.print(" %\\t");              // Tab separator
				Serial.print("Temperature: ");
				Serial.print(temperature);
				Serial.print(" *C ");
				Serial.print("Heat index: ");
				Serial.print(hi);
				Serial.println(" *C");
			}

			void displayData() {
				// Clear entire screen with black background
				tft.fillScreen(TFT_BLACK);
				
				// Display title "Sensor Data" in cyan
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Sensor Data");
				
				// Draw a horizontal line separator
				tft.drawLine(10, 45, 230, 45, TFT_WHITE);
				
				// Display temperature label in yellow
				tft.setTextSize(2);
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);
				tft.setCursor(10, 60);
				tft.print("Temp: ");
				
				// Display temperature value in large red text
				tft.setTextSize(3);
				tft.setTextColor(TFT_RED, TFT_BLACK);
				tft.print(String(temperature, 1));  // Format to 1 decimal place
				
				// Display temperature unit in yellow
				tft.setTextSize(2);
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);
				tft.println(" *C");
				
				// Display humidity label in yellow
				tft.setTextSize(2);
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);
				tft.setCursor(10, 100);
				tft.print("Humidity: ");
				
				// Display humidity value in large blue text
				tft.setTextSize(3);
				tft.setTextColor(TFT_BLUE, TFT_BLACK);
				tft.print(String(humidity, 1));  // Format to 1 decimal place
				
				// Display humidity unit in yellow
				tft.setTextSize(2);
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);
				tft.println(" %");
			}