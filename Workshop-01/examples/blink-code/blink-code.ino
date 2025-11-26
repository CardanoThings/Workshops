// Define the pin number for the LED
			#define LED_PIN 8

			// Setup function is called once when the microcontroller starts up.
			void setup() {
				pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output.
			}

			// Loop function is called repeatedly.
			void loop() {
				digitalWrite(LED_PIN, HIGH); // Turn the LED on (HIGH is the voltage level).
				delay(1000); // Wait for 1 second.
				digitalWrite(LED_PIN, LOW); // Turn the LED off by making the voltage LOW.
				delay(1000); // Wait for 1 second.
			}