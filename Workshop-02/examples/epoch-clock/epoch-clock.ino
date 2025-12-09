// Include necessary libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiClientSecure.h>

// Pin connected to the WS2812 data input
#define LED_PIN 4

// Number of LEDs in the ring (12 LEDs)
#define NUM_LEDS 12

// Total slots in an epoch (approximately 432,000 on Mainnet)
#define SLOTS_PER_EPOCH 432000

// Create NeoPixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Koios API endpoint
const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";

// Variables for timing API calls
unsigned long lastCheck = 0;
const unsigned long checkInterval = 60000;  // Check every minute

// Variables for walking LED - creates a clock-like second hand effect
// The white LED moves around the ring every 5 seconds
// 12 LEDs × 5 seconds = 60 seconds (1 minute) for a full rotation
unsigned long lastWalkUpdate = 0;
const unsigned long walkInterval = 5000;  // Move to next LED every 5 seconds
int walkPosition = 0;  // Current position of walking LED (0-11)

// Store current epoch data
int currentEpoch = 0;
int currentEpochSlot = 0;
int lastEpoch = -1;

void setup() {
    Serial.begin(115200);

    // Initialize LED ring
    strip.begin();
    strip.setBrightness(5);  // Low brightness for safety
    strip.clear();
    strip.show();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Workaround for ESP32-C3 Super Mini

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    // Initial fetch
    fetchEpochData();
    displayProgress();
}

void loop() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        }
    }

    // Check if enough time has passed for API call
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= checkInterval) {
        fetchEpochData();
        displayProgress();
        lastCheck = currentMillis;
    }

    // Update walking LED every 5 seconds (creates second-hand effect)
    if (currentMillis - lastWalkUpdate >= walkInterval) {
        updateWalkingLED();
        lastWalkUpdate = currentMillis;
    }
}

void fetchEpochData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClientSecure client;

        client.setInsecure();
        http.begin(client, apiUrl);

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String response = http.getString();

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, response);

            if (!error && doc.is<JsonArray>() && doc.size() > 0) {
                JsonObject tip = doc[0];
                currentEpoch = tip["epoch_no"] | 0;
                currentEpochSlot = tip["epoch_slot"] | 0;

                // Reset display if epoch changed
                if (currentEpoch != lastEpoch) {
                    lastEpoch = currentEpoch;
                    strip.clear();
                    strip.show();
                    delay(500);
                }
            }
        }

        http.end();
    }
}

void displayProgress() {
    // Calculate epoch progress percentage
    int progressPercent = (currentEpochSlot * 100) / SLOTS_PER_EPOCH;
    if (progressPercent > 100) progressPercent = 100;

    // Calculate how many LEDs should be lit
    int ledsToLight = (progressPercent * NUM_LEDS) / 100;

    // Clear all LEDs
    strip.clear();

    // Light up LEDs based on progress in blue
    for (int i = 0; i < ledsToLight; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 255));  // Blue
    }

    strip.show();
}

void updateWalkingLED() {
    // Display epoch progress first (blue LEDs showing epoch completion)
    displayProgress();

    // Add white walking LED at current position (creates clock second-hand effect)
    // This LED blinks white for 5 seconds at each position before moving
    strip.setPixelColor(walkPosition, strip.Color(255, 255, 255));  // White
    strip.show();

    // Move to next position (wrap around after LED 11 to complete 60-second cycle)
    walkPosition = (walkPosition + 1) % NUM_LEDS;
}