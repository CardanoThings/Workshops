
# ESP32-C3 Koios API Example

This project demonstrates how to connect an ESP32-C3 to WiFi, synchronize time using NTP, make a secure HTTPS API call to the Cardano Koios API, and parse the JSON response using ArduinoJson. The example prints the current epoch_slot from the blockchain tip to the Serial console.

## Features

- Connects to WiFi using credentials from `secrets.h`
- Synchronizes time via NTP (required for HTTPS/TLS)
- Makes an HTTPS GET request to `https://api.koios.rest/api/v1/tip`
- Handles HTTP chunked transfer encoding
- Parses and prints the `epoch_slot` value from the JSON response

## Hardware Requirements

- ESP32-C3 development board

## Library Dependencies

Install these libraries via the Arduino Library Manager:

- **ArduinoJson** by Benoit Blanchon
- **WiFi** (comes with ESP32 core)

## Installation

1. **Clone this repository** or copy the files to your Arduino project folder.
2. **Add your WiFi credentials**:
   - Copy `secrets-example.h` to `secrets.h` and fill in your WiFi SSID and password.
3. **Install the required libraries**:
   - Open Arduino IDE
   - Go to `Sketch` > `Include Library` > `Manage Libraries...`
   - Search for and install `ArduinoJson` by Benoit Blanchon
4. **Select your ESP32-C3 board**:
   - Go to `Tools` > `Board` and select your ESP32-C3 variant
5. **Connect your ESP32-C3 and upload the sketch**

## File Overview

- `ESP32-C3-Epoch.ino`: Main sketch. Handles WiFi, NTP, and API call logic.
- `api_utils.h`: Utility for making HTTPS GET requests and parsing JSON, including chunked transfer decoding.
- `secrets.h`: Your WiFi credentials (not included in version control).
- `secrets-example.h`: Template for your WiFi credentials.

## Serial Output Example

```text
Connecting to WiFi...
..........
WiFi connected!
IP address: 192.168.1.100
Waiting for NTP time sync...
Time synchronized with NTP.
Current time: 2025-08-04 14:35:12 UTC
Calling Koios API...
epoch_slot: 147163
```

## Troubleshooting

- If you see `[api_utils] Connection to host failed`, ensure your WiFi is working and the device time is set (NTP sync succeeded).
- If you see `[api_utils] JSON parse failed`, the API may have changed or the response is not as expected.
- Make sure you have the latest ESP32 Arduino core installed.

## License

MIT
