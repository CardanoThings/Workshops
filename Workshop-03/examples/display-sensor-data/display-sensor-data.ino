// Include necessary libraries
#include <Wire.h>                // I2C communication library (built-in)
#include <Adafruit_AHT10.h>     // Adafruit AHT10 library
#include <Adafruit_GFX.h>       // Adafruit graphics library
#include <Adafruit_SH110X.h>    // Adafruit SH1106 OLED library (for 1.3" OLED)

// OLED display settings
#define SCREEN_WIDTH 128         // OLED display width in pixels
#define SCREEN_HEIGHT 64         // OLED display height in pixels
#define OLED_RESET -1            // Reset pin (not used, set to -1)
#define SCREEN_ADDRESS 0x3C      // I2C address (usually 0x3C or 0x3D)

// Create sensor and display objects
Adafruit_AHT10 aht;             // Initialize AHT10 sensor
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables to store sensor readings
float temperature = 0;         // Current temperature reading
float humidity = 0;            // Current humidity reading

// Variables for timing sensor reads
unsigned long lastRead = 0;                    // Timestamp of last sensor read
const unsigned long readInterval = 2000;        // Read every 2 seconds

void setup() {
    // Initialize serial communication for debugging (115200 baud rate)
    Serial.begin(115200);
    Serial.println("Adafruit AHT10 demo!");
    
    // Initialize AHT10 sensor
    if (!aht.begin()) {
        Serial.println("Could not find AHT10? Check wiring");
        while (1) delay(10);  // Halt if sensor not found
    }
    Serial.println("AHT10 found");
    
    // Initialize OLED display
    if (!display.begin(SCREEN_ADDRESS)) {
        Serial.println("SH1106 allocation failed");
        for (;;);  // Don't proceed, loop forever
    }
    Serial.println("OLED Display initialized!");
    
    // Clear display and show startup message
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("Initializing...");
    display.display();
    delay(1000);
    
    // Clear and show ready message
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Ready!");
    display.display();
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
    // Create sensor event structures to hold readings
    sensors_event_t humidity_event, temp_event;
    
    // Read both temperature and humidity from sensor
    // The getEvent() function populates temp and humidity objects with fresh data
    aht.getEvent(&humidity_event, &temp_event);
    
    // Store readings in global variables
    temperature = temp_event.temperature;        // Temperature in Celsius
    humidity = humidity_event.relative_humidity;  // Humidity as percentage (0-100)
    
    // Print readings to serial monitor for debugging
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("% rH");
}

void displayData() {
    // Clear display buffer
    display.clearDisplay();
    
    // Display temperature label and value
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temperature");
    display.setCursor(0, 14);
    display.setTextSize(3);
    display.print(temperature, 1);  // Format to 1 decimal place
    display.println("C");
    
    // Display humidity label and value
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Humidity: ");
    display.print(humidity, 1);  // Format to 1 decimal place
    display.println("%");
    
    // Update display to show all changes
    display.display();
}
