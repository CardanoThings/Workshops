// Setup function is called once when the microcontroller starts up.
			void setup() {
				pinMode(LED_BUILTIN, OUTPUT); // Set the LED_BUILTIN pin as an output.
			}

			// Loop function is called repeatedly.
			void loop() {
				digitalWrite(LED_BUILTIN, HIGH); // Set the LED_BUILTIN pin to HIGH.
				delay(1000); // Wait for 1 second.
				digitalWrite(LED_BUILTIN, LOW); // Set the LED_BUILTIN pin to LOW.
				delay(1000); // Wait for 1 second.
			}