
#define LED_PIN 8 // Define the onboard LED pin for ESP32-C3 (GPIO8)

// setup() runs once at startup and initializes the LED pin
void setup() { // Runs once at startup
  pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
}

// loop() runs repeatedly, blinking the onboard LED on and off every second
void loop() { // Runs repeatedly after setup
  digitalWrite(LED_PIN, HIGH); // Turn the LED on
  delay(1000); // Wait for 1 second
  digitalWrite(LED_PIN, LOW); // Turn the LED off
  delay(1000); // Wait for 1 second
}
