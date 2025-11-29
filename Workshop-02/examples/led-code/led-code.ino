// Include necessary libraries for WiFi, HTTP requests, and JSON parsing
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials - replace with your network details
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Koios API endpoint for fetching account information
const char* apiUrl = "https://preprod.koios.rest/api/v1/account_info";

// Your Cardano stake address (Preprod Testnet)
String stakeAddress = "stake_test1...";

// GPIO pin connected to LED
const int ledPin = 2;

// Variables for timing balance checks
unsigned long lastCheck = 0;                    // Timestamp of last balance check
const unsigned long checkInterval = 30000;      // Check every 30 seconds (30,000 ms)

// Variables for LED timing
unsigned long ledOnTime = 0;                    // Time when LED was turned on
const unsigned long ledDuration = 60000;        // LED stays on for 60 seconds (60,000 ms)
bool ledActive = false;                         // Track if LED is currently on

// Store previous balance to detect changes
float previousBalance = 0;
bool firstCheck = true;                         // Flag to skip change detection on first check

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Configure LED pin as output
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);  // Start with LED off

    // Start WiFi connection
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Perform initial balance check on startup
    fetchStakeBalance();
}

void loop() {
    // Check if WiFi connection is still active
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost, reconnecting...");
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        }
        Serial.println("Reconnected to WiFi");
    }

    // Get current time in milliseconds
    unsigned long currentMillis = millis();

    // Check if enough time has passed since last balance check
    if (currentMillis - lastCheck >= checkInterval) {
        fetchStakeBalance();
        lastCheck = currentMillis;  // Update last check timestamp
    }

    // Check if LED should be turned off (after 60 seconds)
    if (ledActive && (currentMillis - ledOnTime >= ledDuration)) {
        digitalWrite(ledPin, LOW);  // Turn off LED
        ledActive = false;
        Serial.println("LED turned OFF after 60 seconds");
    }
}

void fetchStakeBalance() {
    // Only proceed if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        Serial.println("\\nFetching stake balance...");

        // Initialize HTTP client with API URL
        http.begin(apiUrl);

        // Set content type header for JSON request
        http.addHeader("Content-Type", "application/json");

        // Create JSON payload with stake address
        // Koios API expects stake addresses in an array under "_stake_addresses" key
        String jsonPayload = "{\\"";
        jsonPayload += "_stake_addresses";
        jsonPayload += "\\":[\\"";
        jsonPayload += stakeAddress;
        jsonPayload += "\\"]}";

        // Send POST request
        int httpResponseCode = http.POST(jsonPayload);

        // Check if request was successful
        if (httpResponseCode > 0) {
            // Get response body as string
            String response = http.getString();
            Serial.println("HTTP Response Code: " + String(httpResponseCode));

            // Create JSON document to parse response
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, response);

            // Check if JSON parsing was successful
            if (!error) {
                // Verify response is an array with at least one element
                if (doc.is<JsonArray>() && doc.size() > 0) {
                    // Get first account info object from array
                    JsonObject accountInfo = doc[0];

                    // Extract total balance as string (Koios returns balance as string)
                    // total_balance includes delegated amount + rewards
                    const char* balanceStr = accountInfo["total_balance"];

                    // Convert string to long long (for large Lovelace values)
                    long long balanceLovelace = 0;
                    if (balanceStr != nullptr) {
                        balanceLovelace = atoll(balanceStr);
                    }

                    // Convert from Lovelace (smallest unit) to tADA (test ADA)
                    // 1 tADA = 1,000,000 Lovelace
                    float balance = balanceLovelace / 1000000.0;

                    // Print account information
                    Serial.println("Stake Address: " + String(accountInfo["stake_address"].as<const char*>()));
                    Serial.print("Total Balance: ");
                    Serial.print(balance, 6);
                    Serial.println(" tADA");

                    // Skip change detection on first check (just store the balance)
                    if (!firstCheck) {
                        // Check if balance changed (up or down)
                        if (balance != previousBalance) {
                            Serial.println("\\n***** BALANCE CHANGE DETECTED! *****");
                            if (balance > previousBalance) {
                                Serial.println("Balance INCREASED (transaction received)");
                            } else {
                                Serial.println("Balance DECREASED (transaction sent)");
                            }

                            // Turn on LED and start timer
                            digitalWrite(ledPin, HIGH);
                            ledOnTime = millis();  // Record current time
                            ledActive = true;
                            Serial.println("LED turned ON for 60 seconds");
                        }
                    } else {
                        firstCheck = false;  // Clear first check flag
                        Serial.println("Initial balance recorded");
                    }

                    // Update previous balance for next comparison
                    previousBalance = balance;
                }
            } else {
                // Print error if JSON parsing failed
                Serial.print("JSON parsing failed: ");
                Serial.println(error.c_str());
            }
        } else {
            // Print error if HTTP request failed
            Serial.println("Error in HTTP request");
            Serial.println("HTTP Response Code: " + String(httpResponseCode));
        }

        // Close HTTP connection
        http.end();
    }
}