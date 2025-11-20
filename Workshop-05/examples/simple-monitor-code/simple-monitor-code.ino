// Simplified payment monitoring
			#include <WiFi.h>
			#include <HTTPClient.h>
			#include <ArduinoJson.h>

			const char* ssid = "Your SSID";
			const char* password = "Your Password";
			const char* apiUrl = "https://preprod.koios.rest/api/v1/address_info";
			
			String address = "addr_test1...";
			float previousBalance = 0.0;
			float expectedAmount = 10.5;

			void setup() {
				Serial.begin(115200);
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
				}
				
				// Get initial balance
				previousBalance = getWalletBalance();
			}

			void loop() {
				delay(5000);  // Check every 5 seconds
				
				float currentBalance = getWalletBalance();
				
				// Check if balance increased by expected amount (or more)
				if (currentBalance > previousBalance) {
					float received = currentBalance - previousBalance;
					
					if (received >= expectedAmount) {
						Serial.println("Payment confirmed!");
						Serial.print("Received: ");
						Serial.print(received, 2);
						Serial.println(" ADA");
					}
					
					previousBalance = currentBalance;
				}
			}

			float getWalletBalance() {
				if (WiFi.status() == WL_CONNECTED) {
					HTTPClient http;
					http.begin(apiUrl);
					http.addHeader("Content-Type", "application/json");
					
					String jsonPayload = "{\"_addresses\":[\"" + address + "\"]}";
					int httpResponseCode = http.POST(jsonPayload);
					
					if (httpResponseCode > 0) {
						String response = http.getString();
						DynamicJsonDocument doc(2048);
						DeserializationError error = deserializeJson(doc, response);
						
						if (!error && doc.is<JsonArray>() && doc.size() > 0) {
							float balance = doc[0]["balance"] | 0.0;
							return balance / 1000000;  // Convert to ADA
						}
					}
					
					http.end();
				}
				return 0.0;
			}