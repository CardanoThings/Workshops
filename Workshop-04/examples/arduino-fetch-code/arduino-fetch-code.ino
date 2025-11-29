// Include necessary libraries
#include <WiFi.h>              // WiFi connectivity
#include <HTTPClient.h>       // HTTP client for API calls
#include <ArduinoJson.h>      // JSON parsing

// WiFi credentials - replace with your network details
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Your Node.js API server URL
// Replace 'your-server-ip' with your computer's IP address
const char* apiUrl = "http://your-server-ip:3000/ticker-data";

// Variables for timing API calls
unsigned long lastFetch = 0;                    // Timestamp of last data fetch
const unsigned long fetchInterval = 30000;      // Fetch every 30 seconds

// Structure to store ticker data
struct TickerData {
    float adaPrice;
    float walletBalance;
    int tokenCount;
} tickerData;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Start WiFi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initial data fetch
    fetchTickerData();
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

    // Check if enough time has passed since last fetch
    if (currentMillis - lastFetch >= fetchInterval) {
        fetchTickerData();
        lastFetch = currentMillis;  // Update last fetch timestamp
    }
}

void fetchTickerData() {
    // Only proceed if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Initialize HTTP client with API URL
        http.begin(apiUrl);

        // Send GET request
        int httpResponseCode = http.GET();

        // Check if request was successful
        if (httpResponseCode > 0) {
            // Get response body as string
            String response = http.getString();
            Serial.println("HTTP Response Code: " + String(httpResponseCode));

            // Create JSON document to parse response (large buffer for complex data)
            DynamicJsonDocument doc(8192);
            DeserializationError error = deserializeJson(doc, response);

            // Check if JSON parsing was successful
            if (!error) {
                // Extract wallet balance
                if (doc.containsKey("wallet")) {
                    tickerData.walletBalance = doc["wallet"]["balance"] | 0.0;
                }

                // Extract ADA price from tokens array
                if (doc.containsKey("tokens") && doc["tokens"].is<JsonArray>()) {
                    JsonArray tokens = doc["tokens"];
                    for (JsonObject token : tokens) {
                        if (token["symbol"] == "ADA") {
                            tickerData.adaPrice = token["price"] | 0.0;
                            break;
                        }
                    }
                }

                // Count number of tokens
                if (doc.containsKey("tokens")) {
                    tickerData.tokenCount = doc["tokens"].size();
                }

                // Print parsed data to serial monitor
                Serial.println("ADA Price: $" + String(tickerData.adaPrice, 2));
                Serial.println("Wallet Balance: " + String(tickerData.walletBalance, 2) + " ADA");
                Serial.println("Token Count: " + String(tickerData.tokenCount));
            } else {
                Serial.print("JSON parsing failed: ");
                Serial.println(error.c_str());
            }
        } else {
            Serial.println("Error in HTTP request");
            Serial.println("HTTP Response Code: " + String(httpResponseCode));
        }

        // Close HTTP connection
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}