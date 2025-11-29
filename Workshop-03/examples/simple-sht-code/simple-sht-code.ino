// Include required libraries
#include <Wire.h>                // I2C communication library (built-in)
#include <SHT2x.h>              // SHT2x sensor library by RobTillaart

// I2C pin configuration
#define SDA_PIN 3               // I2C data line (SDA)
#define SCL_PIN 4               // I2C clock line (SCL)

// Create sensor object
SHT2x sht;

void setup() {
    // Initialize serial communication for debugging (115200 baud rate)
    Serial.begin(115200);

    // Initialize I2C bus with custom pins (SDA pin 3, SCL pin 4)
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize SHT2x sensor
    sht.begin();

    // Print header information
    Serial.println("SHT2x Temperature & Humidity Sensor");
    Serial.println("-----------------------------------");
}

void loop() {
    // Read temperature from sensor (returns value in Celsius)
    float temperature = sht.getTemperature();

    // Read relative humidity from sensor (returns value as percentage)
    float humidity = sht.getHumidity();

    // Check if reads failed (getError() returns 0 for OK, non-zero for errors)
    if (sht.getError() != 0) {
        Serial.println("Failed to read from SHT2x sensor!");
    } else {
        // Print temperature reading with 2 decimal places
        Serial.print("Temperature: ");
        Serial.print(temperature, 2);  // Print with 2 decimal places
        Serial.print(" °C\\t");

        // Print humidity reading with 2 decimal places
        Serial.print("Humidity: ");
        Serial.print(humidity, 2);      // Print with 2 decimal places
        Serial.println(" %RH");        // %RH = Percentage Relative Humidity
    }

    // Print separator line
    Serial.println("-----------------------------------");

    // Wait 2 seconds before next reading
    // This prevents excessive sensor queries and reduces power consumption
    delay(2000);
}