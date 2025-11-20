// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>

			// WiFi credentials - replace with your network details
			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			
			// GPIO pin numbers for hardware control
			const int ledPin = 2;      // GPIO pin connected to LED
			const int relayPin = 4;    // GPIO pin connected to relay module
			
			// Your Cardano wallet address (Preprod Testnet)
			String walletAddress = "addr_test1...";
			
			// Store previous balance to detect changes
			float previousBalance = 0;

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Configure pins as outputs
				pinMode(ledPin, OUTPUT);    // LED pin
				pinMode(relayPin, OUTPUT);  // Relay pin
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
				}
			}

			void loop() {
				// Check balance and control hardware based on changes
				checkAndControl();
				
				// Wait 30 seconds before next check
				delay(30000);
			}

			void checkAndControl() {
				// Fetch current balance from API (simplified - implement fetchBalance())
				float currentBalance = fetchBalance();
				
				// Check if balance increased (new transaction received)
				if (currentBalance > previousBalance) {
					// New funds received - provide visual and hardware feedback
					blinkLED(5);                    // Blink LED 5 times
					digitalWrite(relayPin, HIGH);    // Turn on relay (activate connected device)
				}
				
				// Update previous balance for next comparison
				previousBalance = currentBalance;
			}

			void blinkLED(int times) {
				// Blink LED specified number of times
				for (int i = 0; i < times; i++) {
					digitalWrite(ledPin, HIGH);  // Turn LED on
					delay(200);                 // Wait 200ms
					digitalWrite(ledPin, LOW);   // Turn LED off
					delay(200);                 // Wait 200ms
				}
			}

			float fetchBalance() {
				// Your balance fetching code here
				// This should call the Koios or Blockfrost API
				// and return the current wallet balance
				return 0.0;
			}