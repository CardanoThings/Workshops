// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials - replace with your network details
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Blockfrost API endpoint (Preprod Testnet)
// Note: Blockfrost uses GET requests with stake address in URL path
const char* apiUrl = "https://cardano-preprod.blockfrost.io/api/v0/accounts/";

// Your Blockfrost API key (get free key from blockfrost.io)
const char* apiKey = "your-blockfrost-api-key";

// Your Cardano stake address (Preprod Testnet)
String stakeAddress = "stake_test1...";

// Variables for timing balance checks
unsigned long lastCheck = 0;                    // Timestamp of last balance check
const unsigned long checkInterval = 30000;      // Check every 30 seconds

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Start WiFi connection
    WiFi.begin(ssid, password);

    // Wait until WiFi is connected
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

    // Check if enough time has passed since last check
    if (currentMillis - lastCheck >= checkInterval) {
        fetchStakeBalance();
        lastCheck = currentMillis;  // Update last check timestamp
    }
}

void fetchStakeBalance() {
    // Only proceed if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Build full URL by appending stake address to base URL
        String fullUrl = apiUrl + stakeAddress;

        // Initialize HTTP client with full URL
        http.begin(fullUrl);

        // Blockfrost requires API key in "project_id" header
        http.addHeader("project_id", apiKey);

        // Send GET request (Blockfrost uses GET, not POST like Koios)
        int httpResponseCode = http.GET();

        // Check if request was successful
        if (httpResponseCode > 0) {
            // Get response body as string
            String response = http.getString();

            // Create JSON document to parse response (1024 bytes buffer)
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, response);

        // Check if JSON parsing was successful
        if (!error) {
            // Extract controlled_amount as string (Blockfrost returns balance as string)
            // controlled_amount is the total balance including delegated amount and rewards
            const char* balanceStr = doc["controlled_amount"];

            // Convert string to long long (for large Lovelace values)
            long long balanceLovelace = 0;
            if (balanceStr != nullptr) {
                balanceLovelace = atoll(balanceStr);
            }

            // Convert from Lovelace to tADA (test ADA) - 1 tADA = 1,000,000 Lovelace
            float balance = balanceLovelace / 1000000.0;

            // Print account information
            Serial.println("Stake Address: " + String(doc["stake_address"].as<const char*>()));
            Serial.println("Total Balance: " + String(balance, 6) + " tADA");
        }
        }

        // Close HTTP connection
        http.end();
    }
}