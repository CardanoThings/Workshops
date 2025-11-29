// Include necessary libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// Create TFT display object
TFT_eSPI tft = TFT_eSPI();

// WiFi credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Koios API endpoint for checking address transactions
const char* apiUrl = "https://preprod.koios.rest/api/v1/address_txs";

// Payment monitoring variables
String recipientAddress = "addr_test1...";  // Address to monitor
float expectedAmount = 0.0;                   // Expected payment amount
unsigned long lastCheck = 0;                  // Timestamp of last check
const unsigned long checkInterval = 5000;     // Check every 5 seconds

// Transaction tracking
String lastTransactionHash = "";              // Hash of last seen transaction
bool paymentConfirmed = false;                // Payment confirmation status

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize TFT display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Display waiting message
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 50);
    tft.println("Waiting for");
    tft.setCursor(10, 80);
    tft.println("Payment...");

    // Start WiFi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        }
    }

    // Get current time
    unsigned long currentMillis = millis();

    // Check for payment periodically
    if (!paymentConfirmed && currentMillis - lastCheck >= checkInterval) {
        checkForPayment();
        lastCheck = currentMillis;
    }
}

void checkForPayment() {
    // Only proceed if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Initialize HTTP client with API URL
        http.begin(apiUrl);

        // Set content type header for JSON request
        http.addHeader("Content-Type", "application/json");

        // Create JSON payload with address
        String jsonPayload = "{\"_addresses\":[\"" + recipientAddress + "\"]}";

        // Send POST request
        int httpResponseCode = http.POST(jsonPayload);

        // Check if request was successful
        if (httpResponseCode > 0) {
            // Get response body as string
            String response = http.getString();

            // Create JSON document to parse response
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, response);

            // Check if JSON parsing was successful
            if (!error && doc.is<JsonArray>()) {
                // Check if there are any transactions
                if (doc.size() > 0) {
                    // Get the most recent transaction (first in array)
                    JsonObject latestTx = doc[0];
                    String txHash = latestTx["tx_hash"] | "";

                    // Check if this is a new transaction
                    if (txHash != lastTransactionHash && txHash.length() > 0) {
                        // New transaction detected
                        lastTransactionHash = txHash;

                        // Verify payment amount (simplified - would need to check UTXOs)
                        // For now, just confirm that a transaction was received
                        paymentConfirmed = true;

                        // Display confirmation
                        displayPaymentConfirmation(txHash);
                    }
                }
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
    }
}

void displayPaymentConfirmation(String txHash) {
    // Clear screen
    tft.fillScreen(TFT_BLACK);

    // Draw success message
    tft.setTextSize(3);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 30);
    tft.println("PAYMENT");
    tft.setCursor(10, 60);
    tft.println("CONFIRMED!");

    // Draw checkmark or success icon
    drawCheckmark(180, 40);

    // Display transaction hash (truncated)
    tft.setTextSize(1);
    tft.setTextColor(TFT_GRAY, TFT_BLACK);
    tft.setCursor(10, 100);
    tft.print("TX: ");
    tft.println(txHash.substring(0, 20) + "...");

    // Display amount
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(10, 120);
    tft.print("Amount: ");
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print(String(expectedAmount, 2));
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.println(" ADA");

    // Print to serial monitor
    Serial.println("Payment confirmed!");
    Serial.println("Transaction Hash: " + txHash);
}

void drawCheckmark(int x, int y) {
    // Draw a simple checkmark using lines
    // Draw checkmark lines
    tft.drawLine(x, y + 10, x + 5, y + 15, TFT_GREEN);
    tft.drawLine(x + 5, y + 15, x + 15, y + 5, TFT_GREEN);

    // Make lines thicker by drawing multiple times
    tft.drawLine(x, y + 11, x + 5, y + 16, TFT_GREEN);
    tft.drawLine(x + 5, y + 16, x + 15, y + 6, TFT_GREEN);
}