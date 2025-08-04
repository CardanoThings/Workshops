// api_utils.h
// Utility functions for making HTTPS API calls and parsing JSON responses on ESP32-C3
// Requires WiFi connection and ArduinoJson library

#ifndef API_UTILS_H
#define API_UTILS_H

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Makes an HTTPS GET request to the specified URL and parses the JSON response.
// Parameters:
//   url - The HTTPS URL to call (e.g., "https://api.example.com/data")
//   jsonDoc - Reference to a DynamicJsonDocument to store the parsed JSON
// Returns:
//   true if the request and parsing were successful, false otherwise
bool httpsGetJson(const char* url, DynamicJsonDocument& jsonDoc) {
    WiFiClientSecure client;
    client.setInsecure(); // WARNING: disables certificate validation for simplicity

    // Extract host and path from URL
    String urlStr(url);
    int index = urlStr.indexOf("//");
    if (index < 0) return false;
    String hostAndPath = urlStr.substring(index + 2);
    int slashIndex = hostAndPath.indexOf('/');
    if (slashIndex < 0) return false;
    String host = hostAndPath.substring(0, slashIndex);
    String path = hostAndPath.substring(slashIndex);

    // Connect to host (default HTTPS port 443)
    if (!client.connect(host.c_str(), 443)) {
        Serial.print("[api_utils] Connection to host failed: ");
        Serial.println(host);
        return false;
    }

    // Send HTTP GET request
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP32-C3\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for response
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            client.stop();
            Serial.println("[api_utils] No response from server");
            return false;
        }
    }

    // Read and skip HTTP headers
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r" || line == "\n" || line.length() == 1) {
            break;
        }
    }

    // Handle chunked transfer encoding
    String jsonBody = "";
    while (client.connected()) {
        String chunkSizeStr = client.readStringUntil('\n');
        chunkSizeStr.trim();
        int chunkSize = (int) strtol(chunkSizeStr.c_str(), NULL, 16);
        if (chunkSize == 0) {
            break;
        }
        char chunk[chunkSize + 1];
        int readLen = client.readBytes(chunk, chunkSize);
        chunk[readLen] = '\0';
        jsonBody += String(chunk);
        client.readStringUntil('\n'); // Skip the trailing \r\n after chunk
    }

    // Parse JSON body
    DeserializationError error = deserializeJson(jsonDoc, jsonBody);
    if (error) {
        Serial.println("[api_utils] JSON parse failed. Raw chunked body:");
        Serial.println(jsonBody);
        Serial.print("[api_utils] ArduinoJson error: ");
        Serial.println(error.c_str());
    }
    client.stop();
    return !error;
}

#endif // API_UTILS_H
