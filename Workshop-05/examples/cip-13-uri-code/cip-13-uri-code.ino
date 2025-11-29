// Include necessary libraries
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// WiFi credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Payment details
String recipientAddress = "addr_test1...";  // Address to receive payment
float paymentAmount = 0.0;                   // Payment amount in ADA
String paymentLabel = "";                    // Optional payment label/memo

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
}

void loop() {
    // Your main loop
}

// Function to create CIP-13 payment URI
// Format: web+cardano://[address]?amount=[amount]&label=[label]
String createCIP13URI(float amount, String address, String label = "") {
    // Start with the CIP-13 scheme
    String uri = "web+cardano://";

    // Add recipient address
    uri += address;

    // Add amount parameter (convert ADA to Lovelace: 1 ADA = 1,000,000 Lovelace)
    if (amount > 0) {
        unsigned long lovelace = (unsigned long)(amount * 1000000);
        uri += "?amount=";
        uri += String(lovelace);

        // Add optional label if provided
        if (label.length() > 0) {
            uri += "&label=";
            uri += urlEncode(label);  // URL encode the label
        }
    }

    return uri;
}

// Simple URL encoding function
// Encodes special characters for use in URLs
String urlEncode(String str) {
    String encoded = "";
    for (int i = 0; i < str.length(); i++) {
        char c = str.charAt(i);

        // Encode spaces as %20
        if (c == ' ') {
            encoded += "%20";
        }
        // Encode other special characters
        else if (c == '&') {
            encoded += "%26";
        }
        else if (c == '=') {
            encoded += "%3D";
        }
        else if (c == '?') {
            encoded += "%3F";
        }
        // Keep alphanumeric and safe characters as-is
        else {
            encoded += c;
        }
    }
    return encoded;
}

// Example usage
void generatePaymentURI() {
    // Set payment details
    paymentAmount = 10.5;  // 10.5 ADA
    recipientAddress = "addr_test1q...";
    paymentLabel = "Coffee Payment";

    // Create CIP-13 URI
    String paymentURI = createCIP13URI(paymentAmount, recipientAddress, paymentLabel);

    // Print to serial monitor for debugging
    Serial.println("CIP-13 Payment URI:");
    Serial.println(paymentURI);

    // Example output:
    // web+cardano://addr_test1q...?amount=10500000&label=Coffee%20Payment
}