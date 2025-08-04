

#include <WiFi.h>
#include "secrets.h" // Include WiFi credentials

// setup() runs once at startup
void setup() {
  Serial.begin(115200); // Start the Serial communication
  delay(1000); // Give time for Serial to initialize


  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Start connecting to WiFi

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 20000; // 20 seconds timeout

  // Wait until connected or timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print("."); // Print dot while waiting
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // Print the local IP address
  } else {
    Serial.println("");
    Serial.println("WiFi connection failed! Please check your credentials or network.");
  }
}

// loop() runs repeatedly after setup
void loop() {
  // You can add code here to use the WiFi connection
  delay(1000); // Wait for 1 second
}
