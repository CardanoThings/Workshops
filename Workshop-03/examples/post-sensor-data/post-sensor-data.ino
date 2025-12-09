// Include necessary libraries
#include <WiFi.h>              // WiFi connectivity
#include <HTTPClient.h>       // HTTP client for API calls
#include <ArduinoJson.h>      // JSON parsing and creation
#include <Adafruit_AHT10.h>   // Adafruit AHT10 library

// Create AHT10 sensor object
Adafruit_AHT10 aht;

// WiFi credentials - replace with your network details
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Your API server URL - replace with your server's IP address
const char* apiUrl = "http://YOUR_SERVER_IP:3000/data";

// Variables for timing sensor readings
unsigned long lastReading = 0;                    // Timestamp of last reading
const unsigned long readingInterval = 300000;     // Read every 5 minutes (300000 milliseconds)

// Send once flag - set to true for testing to avoid creating too many transactions
const bool sendOnce = true;                       // If true, send sensor data only once
bool dataSent = false;                            // Track if data has been sent

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    Serial.println("Temperature Sensor NFT Demo!");
    
    // Initialize AHT10 sensor
    if (!aht.begin()) {
        Serial.println("Could not find AHT10? Check wiring");
        while (1) delay(10);  // Halt if sensor not found
    }
    Serial.println("AHT10 found");
    
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
        // Only send if sendOnce is false, or if sendOnce is true and data hasn't been sent yet
        if (!sendOnce || !dataSent) {
            sendSensorData();  // Read sensor and send to API
            dataSent = true;   // Mark that data has been sent
        }
        lastReading = currentMillis;  // Update last reading timestamp
    }
}

void sendSensorData() {
    // Create sensor event structures to hold readings
    sensors_event_t humidity_event, temp_event;
    
    // Read both temperature and humidity from sensor
    // The getEvent() function populates temp and humidity objects with fresh data
    aht.getEvent(&humidity_event, &temp_event);
    
    // Extract temperature and humidity values
    float temperature = temp_event.temperature;        // Temperature in Celsius
    float humidity = humidity_event.relative_humidity;  // Humidity as percentage (0-100)
    
    // Print sensor readings to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("% rH");
    
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
        doc["humidity"] = humidity;          // Humidity as percentage (0-100)
        doc["timestamp"] = millis();         // Current time in milliseconds
        
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

