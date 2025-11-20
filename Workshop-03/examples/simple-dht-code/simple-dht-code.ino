// Include DHT sensor library
			#include <DHT.h>

			// DHT sensor pin configuration
			#define DHTPIN 4               // Digital GPIO pin connected to DHT22 data pin
			#define DHTTYPE DHT22          // Sensor type: DHT22 (also known as AM2302)

			// Create DHT sensor object
			DHT dht(DHTPIN, DHTTYPE);

			void setup() {
				// Initialize serial communication for debugging (115200 baud rate)
				Serial.begin(115200);
				
				// Initialize DHT sensor
				dht.begin();
			}

			void loop() {
				// Wait 2 seconds between measurements
				// DHT22 requires at least 2 seconds between readings for accurate data
				delay(2000);
				
				// Read humidity from sensor (takes ~250ms)
				float h = dht.readHumidity();
				
				// Read temperature from sensor (takes ~250ms)
				float t = dht.readTemperature();
				
				// Check if any reads failed (returns NaN on failure)
				if (isnan(h) || isnan(t)) {
					Serial.println("Failed to read from DHT sensor!");
					return;  // Exit function if read failed
				}
				
				// Print humidity to serial monitor
				Serial.print("Humidity: ");
				Serial.print(h);
				Serial.print(" %\\t");              // Tab separator
				
				// Print temperature to serial monitor
				Serial.print("Temperature: ");
				Serial.print(t);
				Serial.println(" *C");
			}