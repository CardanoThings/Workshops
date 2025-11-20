#include <WiFi.h> // Include the WiFi library for ESP32
            #include <HTTPClient.h> // Include the HTTPClient library for ESP32
            #include <ArduinoJson.h> // Include the ArduinoJSON library for ESP32

            const char* ssid = "Your SSID"; // Your WiFi SSID
            const char* password = "Your Password"; // Your WiFi Password
            const char* apiUrl = "https://preprod.koios.rest/api/v1/tip"; // The API URL for the Koios API

            void setup() {
                Serial.begin(115200); // Initialize the serial communication at 115200 baud rate
                WiFi.begin(ssid, password); // Connect to WiFi using the SSID and Password
                // Wait for the connection to be established
                while (WiFi.status() != WL_CONNECTED) {
                    delay(1000); // Wait for 1 second
                    Serial.println("Connecting to WiFi..."); // Print "Connecting to WiFi..." to the serial monitor
                }
                Serial.println("Connected to WiFi"); // Print "Connected to WiFi" to the serial monitor
                Serial.println("IP address: "); // Print "IP address: " to the serial monitor
                Serial.println(WiFi.localIP()); // Print the IP address to the serial monitor
            }

            void loop() {
                // Check if the WiFi connection is lost
                if (WiFi.status() != WL_CONNECTED) {
                    Serial.println("WiFi connection lost. Reconnecting..."); // Print "WiFi connection lost. Reconnecting..." to the serial monitor
                    WiFi.reconnect(); // Reconnect to WiFi
                    while (WiFi.status() != WL_CONNECTED) {
                        delay(1000); // Wait for 1 second
                        Serial.print("."); // Print "." to the serial monitor
                    }
                    Serial.println("Reconnected!"); // Print "Reconnected!" to the serial monitor
                }
                makeHttpRequest(); // Make the HTTP request
            }

            void makeHttpRequest() {
                if (WiFi.status() == WL_CONNECTED) {
                    HTTPClient http;
                    http.begin(apiUrl);
                    int httpResponseCode = http.GET();
                    if (httpResponseCode > 0) {
                        String response = http.getString();
                        Serial.println("HTTP Response Code: " + String(httpResponseCode));
                        Serial.println("Response:");
                        Serial.println(response);
                        parseJsonResponse(response);
                        Serial.println("Epoch number: " + String(epochNumber));
                    } else {
                        Serial.println("Error in HTTP request");
                        Serial.println("HTTP Response Code: " + String(httpResponseCode));
                    }
                    http.end();
                } else {
                    Serial.println("WiFi not connected");
                }
            }