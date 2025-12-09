// Include required libraries
#include <Adafruit_AHT10.h>

// Create sensor object
Adafruit_AHT10 aht;

void setup() {
    // Initialize serial communication for debugging (115200 baud rate)
    Serial.begin(115200);
    Serial.println("Adafruit AHT10 demo!");

    // Initialize AHT10 sensor
    // begin() returns true if sensor is found, false if not found
    if (!aht.begin()) {
        Serial.println("Could not find AHT10? Check wiring");
        while (1) delay(10);  // Halt execution if sensor not found
    }
    Serial.println("AHT10 found");
}

void loop() {
    // Create sensor event structures to hold readings
    sensors_event_t humidity, temp;
    
    // Read both temperature and humidity simultaneously
    // getEvent() populates temp and humidity objects with fresh data
    aht.getEvent(&humidity, &temp);
    
    // Print temperature reading to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degrees C");
    
    // Print humidity reading to serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("% rH");

    // Wait 500ms before next reading
    delay(500);
}
