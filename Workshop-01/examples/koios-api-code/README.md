# Koios API Code

Example of making HTTP requests to the Koios API to fetch Cardano blockchain data

## Overview

This sketch demonstrates how to connect an ESP32 to WiFi, make HTTPS requests to a REST API (Koios), and parse JSON responses. It fetches the current epoch number from the Cardano preprod testnet and displays it in the Serial Monitor. This is a fundamental example for building IoT devices that interact with blockchain APIs.

## Requirements

- ESP32 or ESP8266 based microcontroller
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- **ArduinoJSON library** (must be installed via Library Manager)
- USB cable for programming and power

## How It Works

### Code Structure

The sketch is organized into several key functions:
- **`setup()`**: Initializes serial communication and connects to WiFi
- **`loop()`**: Continuously monitors WiFi connection and makes API requests
- **`makeHttpRequest()`**: Sends HTTP GET request to Koios API
- **`parseJsonResponse()`**: Parses the JSON response and extracts data

### Understanding the Code

#### Libraries

1. **`#include <WiFi.h>`**
   - Provides WiFi connectivity functions
   - Part of ESP32 Arduino core (no installation needed)

2. **`#include <HTTPClient.h>`**
   - Provides HTTP/HTTPS client functionality
   - Part of ESP32 Arduino core (no installation needed)
   - Handles GET, POST, PUT, DELETE requests

3. **`#include <ArduinoJson.h>`**
   - **Must be installed separately** via Library Manager
   - Parses JSON responses from APIs
   - Essential for working with REST APIs that return JSON

#### Configuration Variables

4. **WiFi Credentials**
   ```cpp
   const char* ssid = "Your SSID";      // Your WiFi network name
   const char* password = "Your Password"; // Your WiFi password
   ```
   - **Important**: Replace with your actual WiFi credentials before uploading!

5. **API URL**
   ```cpp
   const char* apiUrl = "https://preprod.koios.rest/api/v1/tip";
   ```
   - Points to Koios preprod (testnet) Chain Tip endpoint
   - Returns current blockchain tip information
   - For mainnet, use: `https://api.koios.rest/api/v1/tip`

6. **Data Storage**
   ```cpp
   int epochNumber = 0;
   ```
   - Stores the parsed epoch number from the API response

#### Setup Function

7. **Serial Communication**
   ```cpp
   Serial.begin(115200);
   ```
   - Initializes serial communication at 115200 baud
   - Required to see output in Serial Monitor
   - **Note**: Set Serial Monitor to 115200 baud to view messages

8. **WiFi Connection**
   ```cpp
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   ```
   - Starts WiFi connection process
   - Waits until connection is established
   - Prints connection status to Serial Monitor
   - Displays assigned IP address when connected

#### JSON Parsing Function

9. **`parseJsonResponse(String response)`**
   ```cpp
   JsonDocument doc;
   DeserializationError error = deserializeJson(doc, response);
   ```
   - Creates a JSON document to hold parsed data
   - **`deserializeJson()`**: Converts JSON string into structured data
   - Handles parsing errors gracefully

10. **Error Handling**
    ```cpp
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }
    ```
    - Checks if JSON parsing succeeded
    - Prints error message if parsing fails
    - Prevents crashes from malformed JSON

11. **Data Extraction**
    ```cpp
    epochNumber = doc[0]["epoch_no"];
    ```
    - Koios API returns an **array** with one object: `[{...}]`
    - `doc[0]` accesses the first (and only) element
    - `["epoch_no"]` extracts the epoch number field
    - Stores value in the `epochNumber` variable

#### Loop Function

12. **WiFi Monitoring**
    ```cpp
    if (WiFi.status() != WL_CONNECTED) {
        // Reconnection logic
    }
    ```
    - Continuously checks WiFi connection status
    - Automatically reconnects if connection is lost
    - Ensures device stays online

13. **API Request and Delay**
    ```cpp
    makeHttpRequest();
    delay(60000); // 60 seconds
    ```
    - Calls function to make HTTP request
    - Waits 60 seconds before next request
    - **Rate limiting**: Prevents overwhelming the API
    - Adjust delay based on your needs (but be respectful!)

#### HTTP Request Function

14. **Connection Check**
    ```cpp
    if (WiFi.status() == WL_CONNECTED) {
        // Make request
    }
    ```
    - Only makes request if WiFi is connected
    - Prevents errors from attempting requests without network

15. **HTTP Client Setup**
    ```cpp
    HTTPClient http;
    http.begin(apiUrl);
    ```
    - Creates HTTP client object
    - Initializes with the API URL
    - Prepares for HTTPS request (handles SSL/TLS automatically)

16. **Sending Request**
    ```cpp
    int httpResponseCode = http.GET();
    ```
    - Sends HTTP GET request to the API
    - Returns response code:
      - **200**: Success (OK)
      - **-1**: Connection error
      - **Other codes**: HTTP error (404, 500, etc.)

17. **Reading Response**
    ```cpp
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response Code: " + String(httpResponseCode));
        Serial.println("Response:");
        Serial.println(response);
    }
    ```
    - Checks if request was successful (response code > 0)
    - **`http.getString()`**: Retrieves the full response body
    - Prints response code and full JSON response
    - Useful for debugging and understanding API structure

18. **Parsing and Displaying**
    ```cpp
    parseJsonResponse(response);
    Serial.println("Epoch number: " + String(epochNumber));
    ```
    - Calls parsing function to extract epoch number
    - Displays the extracted value
    - Shows only the data we care about (epoch number)

19. **Cleanup**
    ```cpp
    http.end();
    ```
    - Closes HTTP connection
    - Frees resources
    - Important for preventing memory leaks

### JSON Response Structure

The Koios API returns data in this format:

```json
[{
  "hash": "14c6413b8df915c58d9da162cf22ad58dc52834c8ce7105fe91d08e804cb5a36",
  "epoch_no": 252,
  "abs_slot": 107460097,
  "epoch_slot": 237697,
  "block_height": 4122947,
  "block_no": 4122947,
  "block_time": 1763143297
}]
```

**Key points:**
- Response is an **array** (starts with `[`)
- Contains one **object** (starts with `{`)
- `epoch_no` is the field we extract
- Other fields contain additional blockchain information

## Installation

### Step 1: Install ArduinoJSON Library

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. In the search box, type **ArduinoJson**
4. Find **"ArduinoJson" by Benoit Blanchon** (the one with millions of downloads)
5. Click **Install**
6. Wait for installation to complete

**Note**: The HTTPClient library comes pre-installed with ESP32 board support, so no additional installation needed.

### Step 2: Configure WiFi Credentials

**Before uploading**, update these lines with your actual WiFi credentials:

```cpp
const char* ssid = "Your SSID";        // Change to your WiFi name
const char* password = "Your Password"; // Change to your WiFi password
```

### Step 3: Upload Code

1. Copy the code from `koios-api-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor: **Tools → Serial Monitor** (or `Ctrl+Shift+M` / `Cmd+Shift+M`)
7. Set baud rate to **115200** in the Serial Monitor
8. Watch the API requests!

## Usage

After uploading, the Serial Monitor will show:

1. **WiFi Connection Process**:
   ```
   Connecting to WiFi...
   Connecting to WiFi...
   Connected to WiFi
   IP address: 
   192.168.1.XXX
   ```

2. **API Response** (every 60 seconds):
   ```
   HTTP Response Code: 200
   Response:
   [{"hash":"...","epoch_no":252,...}]
   Epoch number: 252
   ```

The device will continuously:
- Monitor WiFi connection
- Make API requests every 60 seconds
- Parse and display the epoch number
- Automatically reconnect if WiFi drops

## Understanding the Output

### HTTP Response Codes

- **200**: Success - Request completed successfully
- **-1**: Connection error - Couldn't reach the server (check WiFi/internet)
- **404**: Not Found - Endpoint doesn't exist
- **500**: Server Error - API server problem
- **Other codes**: See [HTTP status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status)

### Response Fields

The Koios Chain Tip endpoint returns:
- **`hash`**: Block hash (unique identifier)
- **`epoch_no`**: Current epoch number (what we extract)
- **`abs_slot`**: Absolute slot number
- **`epoch_slot`**: Slot within current epoch
- **`block_height`**: Current block height
- **`block_no`**: Block number
- **`block_time`**: Unix timestamp of block

## Troubleshooting

### "Failed to parse JSON" Error

- **Check Serial Monitor output**: Look at the raw response
- **Verify JSON structure**: Make sure API returned valid JSON
- **ArduinoJSON version**: Ensure you have a recent version installed
- **Memory issues**: ESP32 might be running out of memory for large responses

### HTTP Response Code: -1

- **WiFi not connected**: Check WiFi credentials and signal strength
- **Internet connection**: Verify your router has internet access
- **Firewall blocking**: Some networks block HTTPS requests
- **API server down**: Check if [preprod.koios.rest](https://preprod.koios.rest/) is accessible

### "WiFi not connected" Messages

- **Wrong credentials**: Double-check SSID and password
- **Out of range**: Move ESP32 closer to router
- **5GHz network**: ESP32 only supports 2.4GHz WiFi
- **Router blocking**: Some routers block new devices

### No Response in Serial Monitor

- **Wrong baud rate**: Must be set to 115200
- **Wrong port**: Select correct COM port in Tools → Port
- **USB cable**: Ensure it's a data cable, not just charging

### Memory Issues

If you see crashes or strange behavior:
- **Reduce JSON buffer size**: ArduinoJSON uses memory for parsing
- **Check available memory**: Use `ESP.getFreeHeap()` to debug
- **Simplify response**: Only parse the fields you need

## Customization

### Change API Endpoint

To use a different Koios endpoint:

```cpp
const char* apiUrl = "https://preprod.koios.rest/api/v1/YOUR_ENDPOINT";
```

### Extract Different Data

To extract other fields from the response:

```cpp
// In parseJsonResponse function:
int blockHeight = doc[0]["block_height"];
String blockHash = doc[0]["hash"].as<String>();
unsigned long blockTime = doc[0]["block_time"];
```

### Change Request Interval

To make requests more or less frequently:

```cpp
delay(30000);  // 30 seconds
delay(120000); // 2 minutes
```

**Note**: Be respectful of API rate limits. 60 seconds is a safe default.

### Use Mainnet Instead of Testnet

Change the API URL:

```cpp
const char* apiUrl = "https://api.koios.rest/api/v1/tip";
```

## Next Steps

Once you've mastered this example, try:

- **Multiple endpoints**: Fetch data from different Koios endpoints
- **POST requests**: Send data to APIs (requires different endpoints)
- **Error handling**: Add retry logic for failed requests
- **Data logging**: Store responses in files or databases
- **Display data**: Show results on TFT displays or web interfaces
- **Real-time updates**: Use WebSockets for live data (advanced)

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/01-basics/api-setup)

## Additional Resources

- [Koios API Documentation](https://preprod.koios.rest/) - Full API reference
- [ArduinoJSON Documentation](https://arduinojson.org/) - JSON parsing guide
- [ESP32 HTTPClient Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient) - HTTP client docs
- [REST API Tutorial](https://www.restapitutorial.com/) - Learn about REST APIs
- [JSON Viewer Extension](https://github.com/rbrahul/Awesome-JSON-Viewer) - Better JSON viewing in browser

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit WiFi credentials to version control** (Git)
- **Use environment variables or separate config files** for production code
- **HTTPS is automatic**: ESP32 HTTPClient handles SSL/TLS encryption
- **API keys**: Some APIs require authentication - store keys securely
- **Rate limiting**: Respect API rate limits to avoid being blocked
