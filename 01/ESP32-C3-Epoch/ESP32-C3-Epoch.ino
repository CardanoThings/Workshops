


#include <WiFi.h>
#include "secrets.h" // Include WiFi credentials
#include "api_utils.h" // Include the API utility
#include <ArduinoJson.h> // Include ArduinoJson for JSON parsing

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

    // Set time via NTP for TLS
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Waiting for NTP time sync");
    time_t now = time(nullptr);
    int ntpTimeout = 10; // seconds
    while (now < 8 * 3600 * 2 && ntpTimeout-- > 0) {
      delay(1000);
      Serial.print(".");
      now = time(nullptr);
    }
    Serial.println("");
    if (now < 8 * 3600 * 2) {
      Serial.println("Failed to sync time with NTP!");
    } else {
      Serial.println("Time synchronized with NTP.");
      // Print the current date/time
      struct tm timeinfo;
      gmtime_r(&now, &timeinfo);
      char buf[64];
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
      Serial.print("Current time: ");
      Serial.println(buf);
    }
  } else {
    Serial.println("");
    Serial.println("WiFi connection failed! Please check your credentials or network.");
  }
}

// loop() runs repeatedly after setup
void loop() {
  static bool apiCalled = false;
  if (!apiCalled) {
    if (WiFi.status() == WL_CONNECTED) {
      // Prepare a DynamicJsonDocument for the response (adjust size as needed)
      DynamicJsonDocument doc(2048);
      const char* url = "https://api.koios.rest/api/v1/tip";
      Serial.println("Calling Koios API...");
      if (httpsGetJson(url, doc)) {
        // Print only the epoch_slot from the first array element
        if (doc.is<JsonArray>() && doc.size() > 0 && doc[0]["epoch_slot"]) {
          long epoch_slot = doc[0]["epoch_slot"];
          Serial.print("epoch_slot: ");
          Serial.println(epoch_slot);
        } else {
          Serial.println("Could not find epoch_slot in response.");
        }
      } else {
        Serial.println("API call or JSON parse failed.");
      }
      apiCalled = true;
    } else {
      Serial.println("WiFi not connected. Skipping API call.");
      delay(1000); // Wait and retry WiFi check
    }
  }
  // Do nothing after the call
}
