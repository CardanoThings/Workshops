// Include the Adafruit NeoPixel library
#include <Adafruit_NeoPixel.h>

// Pin connected to the WS2812 data input
#define LED_PIN 4

// Number of LEDs in the ring (12 LEDs)
#define NUM_LEDS 12

// Create NeoPixel object
// Parameter 1 = number of pixels
// Parameter 2 = pin number
// Parameter 3 = pixel type flags (NEO_GRB + NEO_KHZ800 for WS2812)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Initialize the NeoPixel ring
    strip.begin();

    // Set brightness to a very low value (5 out of 255) to protect ESP32-C3
    // This is approximately 2% brightness - safe for USB power
    strip.setBrightness(5);

    // Clear all LEDs (turn them all off)
    strip.clear();

    // Update the strip to apply changes
    strip.show();

    Serial.println("LED Ring initialized. Starting blink sequence...");
}

void loop() {
    // Loop through all 12 LEDs one at a time
    for (int i = 0; i < NUM_LEDS; i++) {
        // Clear all LEDs first
        strip.clear();

        // Set the current LED to white (R=255, G=255, B=255)
        // The brightness is already limited by setBrightness(5) in setup()
        strip.setPixelColor(i, strip.Color(255, 255, 255));

        // Update the strip to show the change
        strip.show();

        // Print which LED is lit
        Serial.print("LED ");
        Serial.print(i);
        Serial.println(" ON");

        // Wait 200 milliseconds before moving to next LED
        delay(200);
    }

    // After all LEDs have been lit, clear the display
    strip.clear();
    strip.show();
}