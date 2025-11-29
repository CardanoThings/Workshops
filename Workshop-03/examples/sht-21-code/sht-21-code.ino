// Include necessary libraries
#include <Wire.h>                // I2C communication library (built-in)
#include <SHT2x.h>              // SHT2x sensor library by RobTillaart
#include <TFT_eSPI.h>          // TFT display library
#include <SPI.h>               // SPI communication for display

// Create sensor and display objects
SHT2x sht;                      // Initialize SHT2x sensor
TFT_eSPI tft = TFT_eSPI();     // Create TFT display object

// Variables to store sensor readings
float temperature = 0;         // Current temperature reading
float humidity = 0;            // Current humidity reading

// I2C pin configuration
#define SDA_PIN 3               // I2C data line (SDA)
#define SCL_PIN 4               // I2C clock line (SCL)

// Variables for timing sensor reads
unsigned long lastRead = 0;                    // Timestamp of last sensor read
const unsigned long readInterval = 2000;        // Read every 2 seconds

void setup() {
    // Initialize serial communication for debugging (115200 baud rate)
    Serial.begin(115200);

    // Initialize I2C bus with custom pins (SDA pin 3, SCL pin 4)
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize SHT2x sensor
    sht.begin();

    // Initialize TFT display
    tft.init();                        // Initialize display hardware
    tft.setRotation(1);                // Set to landscape orientation
    tft.fillScreen(TFT_BLACK);         // Clear screen with black background
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  // White text on black background
    tft.setTextSize(2);                // Set default text size

    // Display startup message on screen
    tft.setCursor(10, 10);
    tft.println("Initializing...");
    delay(1000);

    // Clear screen and show ready message
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Ready!");
    delay(500);
}

void loop() {
    // Get current time in milliseconds
    unsigned long currentMillis = millis();

    // Check if enough time has passed since last sensor read
    if (currentMillis - lastRead >= readInterval) {
        readSensorData();  // Read from sensor
        displayData();     // Update display
        lastRead = currentMillis;  // Update last read timestamp
    }
}

void readSensorData() {
    // Read temperature from sensor (returns value in Celsius)
    float t = sht.getTemperature();

    // Read relative humidity from sensor (returns value as percentage)
    float h = sht.getHumidity();

    // Check if reads failed (getError() returns 0 for OK, non-zero for errors)
    if (sht.getError() != 0) {
        Serial.println("Failed to read from SHT2x sensor!");
        return;  // Exit function if read failed
    }

    // Store readings in global variables
    temperature = t;
    humidity = h;

    // Print readings to serial monitor for debugging
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);  // Print with 2 decimal places
    Serial.print(" °C\\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);      // Print with 2 decimal places
    Serial.println(" %RH");
}

void displayData() {
    // Clear entire screen with black background
    tft.fillScreen(TFT_BLACK);

    // Display title "Sensor Data" in cyan
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Sensor Data");

    // Draw a horizontal line separator
    tft.drawLine(10, 45, 230, 45, TFT_WHITE);

    // Display temperature label in yellow
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 60);
    tft.print("Temp: ");

    // Display temperature value in large red text
    tft.setTextSize(3);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print(String(temperature, 1));  // Format to 1 decimal place

    // Display temperature unit in yellow
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(" °C");

    // Display humidity label in yellow
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 100);
    tft.print("Humidity: ");

    // Display humidity value in large blue text
    tft.setTextSize(3);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.print(String(humidity, 1));  // Format to 1 decimal place

    // Display humidity unit in yellow
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(" %RH");
}