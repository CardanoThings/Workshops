// Simple Relay Blink Example
// This example demonstrates basic relay control without any network connectivity
// Perfect for testing your relay wiring before adding blockchain integration
//
// Wiring:
//   VCC -> 3.3V or 5V (check your relay module specifications)
//   GND -> GND
//   IN  -> GPIO 4 (or any available GPIO pin)
//
// Note: Most relay modules are active LOW (LOW = ON, HIGH = OFF)
// If your relay doesn't work, try reversing HIGH and LOW

// Define the GPIO pin connected to the relay IN pin
const int relayPin = 4;  // Change this to match your wiring

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Wait for serial port to initialize
    delay(1000);

    // Configure relay pin as output
    pinMode(relayPin, OUTPUT);

    // Set relay to OFF state initially
    // For active LOW relays: HIGH = OFF, LOW = ON
    // For active HIGH relays: LOW = OFF, HIGH = ON
    // Try both if unsure - you'll hear a click when the relay activates
    digitalWrite(relayPin, HIGH);  // Start with relay OFF

    Serial.println("Relay Blink Example");
    Serial.println("Relay will turn ON for 2 seconds, then OFF for 2 seconds");
    Serial.println("You should hear a click when the relay activates");
}

void loop() {
    // Turn relay ON
    // For active LOW: set pin to LOW
    // For active HIGH: set pin to HIGH
    Serial.println("Relay ON");
    digitalWrite(relayPin, LOW);  // LOW activates active LOW relays
    delay(2000);                  // Keep relay ON for 2 seconds

    // Turn relay OFF
    // For active LOW: set pin to HIGH
    // For active HIGH: set pin to LOW
    Serial.println("Relay OFF");
    digitalWrite(relayPin, HIGH); // HIGH deactivates active LOW relays
    delay(2000);                  // Keep relay OFF for 2 seconds

    // This creates a continuous 2-second ON/OFF cycle
    // You should hear the relay clicking every 2 seconds
}