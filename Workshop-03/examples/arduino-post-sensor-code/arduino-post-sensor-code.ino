// Include necessary libraries
#include <WiFi.h>              // WiFi connectivity
#include <HTTPClient.h>       // HTTP client for API calls
#include <ArduinoJson.h>      // JSON parsing and creation
#include <SHT2x.h>            // SHT21 sensor library

// I2C pins for SHT21 sensor
#define SDA_PIN 3             // I2C data line
#define SCL_PIN 4              // I2C clock line

// Create SHT2x sensor object
SHT2x sht;

// WiFi credentials - replace with your network details
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Your API server URL - replace with your server's IP address
const char* apiUrl = "http://YOUR_SERVER_IP:3000/sensor-data";

// Variables for timing sensor readings
unsigned long lastReading = 0;                    // Timestamp of last reading
const unsigned long readingInterval = 10000;      // Read every 10 seconds (10000 milliseconds)

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize I2C bus
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize SHT21 sensor
    sht.begin();

    // Start WiFi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
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

    // Check if enough time has passed since last reading
    if (currentMillis - lastReading >= readingInterval) {
        sendSensorData();  // Read sensor and send to API
        lastReading = currentMillis;  // Update last reading timestamp
    }
}

void sendSensorData() {
    // Read temperature from SHT21 sensor
    float temperature = sht.getTemperature();

    // Read humidity from SHT21 sensor
    float humidity = sht.getHumidity();

    // Check if sensor reads failed
    if (sht.getError() != 0) {
        Serial.println("Failed to read from SHT21 sensor!");
        return;  // Exit function if read failed
    }

    // Print sensor readings to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %RH");

    // Only proceed if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Initialize HTTP client with API URL
        http.begin(apiUrl);

        // Set content type header for JSON request
        http.addHeader("Content-Type", "application/json");

        // Create JSON document to build request payload
        DynamicJsonDocument doc(512);

        // Add sensor data to JSON document
        doc["temperature"] = temperature;    // Temperature in Celsius
        doc["humidity"] = humidity;          // Humidity in %RH
        doc["timestamp"] = millis();         // Current time in milliseconds
        doc["sensor_type"] = "SHT21";        // Sensor type identifier

        // Serialize JSON document to string
        String jsonPayload;
        serializeJson(doc, jsonPayload);

        Serial.println("Sending data to API...");
        Serial.println("Payload: " + jsonPayload);

        // Send POST request to API
        int httpResponseCode = http.POST(jsonPayload);

        // Check if request was successful
        if (httpResponseCode > 0) {
            // Get response body
            String response = http.getString();
            Serial.println("HTTP Response Code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        } else {
            // Print error if request failed
            Serial.println("Error in HTTP request");
            Serial.println("HTTP Response Code: " + String(httpResponseCode));
        }

        // Close HTTP connection
        http.end();
    }
}