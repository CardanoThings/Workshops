# Wallet Balance Code

Fetch Cardano stake address balance from Koios API using POST requests

## Overview

This sketch demonstrates how to fetch your Cardano stake address balance from the Koios API using a POST request. It connects to WiFi, sends your stake address to the Koios `/account_info` endpoint, parses the JSON response, and displays your balance in the Serial Monitor. The code also includes balance change detection to notify you when your balance increases or decreases.

## Requirements

- ESP32 or ESP8266 based microcontroller
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- **ArduinoJSON library** (must be installed via Library Manager)
- Cardano stake address (Preprod Testnet)
- USB cable for programming and power

## How It Works

### Code Structure

The sketch is organized into several key functions:
- **`setup()`**: Initializes serial communication, connects to WiFi, and performs initial balance check
- **`loop()`**: Monitors WiFi connection and calls balance fetch function every 30 seconds
- **`fetchStakeBalance()`**: Creates JSON payload, sends POST request, and parses response

### Understanding the Code

#### Libraries

1. **`#include <WiFi.h>`**
   - Provides WiFi connectivity functions
   - Part of ESP32 Arduino core (no installation needed)

2. **`#include <HTTPClient.h>`**
   - Provides HTTP/HTTPS client functionality
   - Part of ESP32 Arduino core (no installation needed)
   - Handles POST requests with JSON payloads

3. **`#include <ArduinoJson.h>`**
   - **Must be installed separately** via Library Manager
   - Parses JSON responses from APIs
   - Essential for working with REST APIs

#### Configuration Variables

4. **WiFi Credentials**
   ```cpp
   const char* ssid = "Your SSID";
   const char* password = "Your Password";
   ```
   - **Important**: Replace with your actual WiFi credentials before uploading!

5. **API Endpoint**
   ```cpp
   const char* apiUrl = "https://preprod.koios.rest/api/v1/account_info";
   ```
   - Koios API endpoint for fetching account information
   - Uses Preprod Testnet (for mainnet, use `https://api.koios.rest/api/v1/account_info`)

6. **Stake Address**
   ```cpp
   String stakeAddress = "stake_test1...";
   ```
   - **Important**: Replace with your actual Cardano stake address
   - Must be a Preprod Testnet address (starts with `stake_test1`)
   - Get your stake address from Yoroi wallet (Receive tab → Rewards section)

7. **Timing Variables**
   ```cpp
   unsigned long lastCheck = 0;
   const unsigned long checkInterval = 30000; // 30 seconds
   ```
   - Controls how often balance is checked
   - Uses non-blocking timing (doesn't freeze the device)
   - Adjust interval based on your needs (but respect API rate limits)

8. **Balance Tracking**
   ```cpp
   float previousBalance = 0;
   ```
   - Stores previous balance to detect changes
   - Used to notify when balance increases or decreases

#### Setup Function

9. **Serial Communication**
   ```cpp
   Serial.begin(115200);
   ```
   - Initializes serial communication at 115200 baud
   - **Note**: Set Serial Monitor to 115200 baud to view messages

10. **WiFi Connection**
    ```cpp
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    ```
    - Connects to WiFi network
    - Waits until connection is established
    - Displays connection status and IP address

11. **Initial Balance Check**
    ```cpp
    fetchStakeBalance();
    ```
    - Fetches balance immediately on startup
    - Provides instant feedback that the device is working

#### Loop Function

12. **WiFi Monitoring**
    ```cpp
    if (WiFi.status() != WL_CONNECTED) {
        // Reconnection logic
    }
    ```
    - Continuously checks WiFi connection
    - Automatically reconnects if connection is lost
    - Ensures device stays online

13. **Timed Balance Checks**
    ```cpp
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= checkInterval) {
        fetchStakeBalance();
        lastCheck = currentMillis;
    }
    ```
    - Uses `millis()` for non-blocking timing
    - Checks balance every 30 seconds
    - Doesn't freeze the device (unlike `delay()`)

#### Fetch Balance Function

14. **HTTP Client Setup**
    ```cpp
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    ```
    - Creates HTTP client object
    - Initializes with API URL
    - Sets content type header (required for JSON POST requests)

15. **JSON Payload Creation**
    ```cpp
    String jsonPayload = "{\"";
    jsonPayload += "_stake_addresses";
    jsonPayload += "\":[\"";
    jsonPayload += stakeAddress;
    jsonPayload += "\"]}";
    ```
    - Builds JSON payload step by step to avoid escaping issues
    - Koios API expects format: `{"_stake_addresses": ["stake_test1..."]}`
    - Can include multiple addresses in the array

16. **POST Request**
    ```cpp
    int httpResponseCode = http.POST(jsonPayload);
    ```
    - Sends POST request with JSON payload
    - Returns HTTP response code:
      - **200**: Success
      - **-1**: Connection error
      - **Other codes**: HTTP errors

17. **Response Handling**
    ```cpp
    if (httpResponseCode > 0) {
        String response = http.getString();
        // Parse JSON...
    }
    ```
    - Checks if request was successful
    - Retrieves response body as string
    - Response contains account information array

18. **JSON Parsing**
    ```cpp
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);
    ```
    - Creates JSON document with 2048 byte buffer
    - Parses JSON response string
    - Handles parsing errors gracefully

19. **Array Validation**
    ```cpp
    if (doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject accountInfo = doc[0];
    }
    ```
    - Verifies response is an array
    - Checks array has at least one element
    - Accesses first account info object

20. **Balance Extraction**
    ```cpp
    const char* balanceStr = accountInfo["total_balance"];
    long long balanceLovelace = atoll(balanceStr);
    ```
    - Koios returns balance as **string** (to handle large numbers)
    - **`atoll()`**: Converts string to `long long` integer
    - Necessary because Cardano balances can be very large

21. **Unit Conversion**
    ```cpp
    float balance = balanceLovelace / 1000000.0;
    ```
    - Converts from Lovelace to tADA (test ADA)
    - **1 tADA = 1,000,000 Lovelace**
    - Uses float for decimal precision

22. **Display Balance**
    ```cpp
    Serial.println("Stake Address: " + String(accountInfo["stake_address"]));
    Serial.println("Total Balance: " + String(balance, 6) + " tADA");
    ```
    - Prints stake address and balance
    - Balance displayed with 6 decimal places
    - Easy to read format

23. **Balance Change Detection**
    ```cpp
    if (balance != previousBalance) {
        if (balance > previousBalance) {
            Serial.println("✓ Balance increased!");
        } else {
            Serial.println("✓ Balance decreased!");
        }
        previousBalance = balance;
    }
    ```
    - Compares current balance with previous balance
    - Notifies when balance changes
    - Updates stored balance for next comparison

### API Request Format

The Koios API expects a POST request with this JSON structure:

**Request:**
```json
{
  "_stake_addresses": [
    "stake_test1urq4rcynzj4uxqc74c852zky7wa6epgmn9r6k3j3gv7502q8jks0l"
  ]
}
```

**Response:**
```json
[
  {
    "stake_address": "stake_test1uz22g9jd408t8zq8g3dw8p60qla49qjgvhh7z74kjpvuyrctlwf4m",
    "status": "registered",
    "total_balance": "10497440929",
    "utxo": "10497440929",
    "rewards": "0",
    "rewards_available": "0",
    "delegated_pool": null,
    "delegated_drep": "drep1ytesfw7n2pq5ys2rk0m7fxxd2dyagf820wy24d82rdd9yxqfm4qjg"
  }
]
```

**Key Response Fields:**
- **`stake_address`**: Your stake address
- **`total_balance`**: Total balance in Lovelace (as string)
- **`utxo`**: UTXO value in Lovelace
- **`rewards`**: Total rewards earned
- **`rewards_available`**: Available rewards to withdraw
- **`delegated_pool`**: Pool ID if delegated (null if not)
- **`delegated_drep`**: DRep ID for governance voting

## Installation

### Step 1: Install ArduinoJSON Library

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. In the search box, type **ArduinoJson**
4. Find **"ArduinoJson" by Benoit Blanchon** (the one with millions of downloads)
5. Click **Install**
6. Wait for installation to complete

### Step 2: Get Your Stake Address

1. Open your Yoroi wallet extension
2. Make sure you're on the **Preprod Testnet** (orange banner at the top)
3. Go to the **"Wallet"** section
4. Click on the **"Receive"** tab
5. Copy your stake address from the **rewards section** (starts with `stake_test1...`)

### Step 3: Configure the Code

**Before uploading**, update these lines:

```cpp
const char* ssid = "Your SSID";        // Change to your WiFi name
const char* password = "Your Password"; // Change to your WiFi password
String stakeAddress = "stake_test1..."; // Change to your stake address
```

### Step 4: Upload Code

1. Copy the code from `wallet-balance.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor: **Tools → Serial Monitor** (or `Ctrl+Shift+M` / `Cmd+Shift+M`)
7. Set baud rate to **115200** in the Serial Monitor
8. Watch your balance updates!

## Usage

After uploading, the Serial Monitor will show:

1. **WiFi Connection**:
   ```
   Connecting to WiFi...
   Connected to WiFi
   IP address: 192.168.1.XXX
   ```

2. **Initial Balance** (on startup):
   ```
   HTTP Response Code: 200
   Stake Address: stake_test1...
   Total Balance: 10.497441 tADA
   ```

3. **Periodic Updates** (every 30 seconds):
   ```
   Stake Address: stake_test1...
   Total Balance: 10.497441 tADA
   ```

4. **Balance Change Detection**:
   ```
   Stake Address: stake_test1...
   Total Balance: 15.497441 tADA
   ✓ Balance increased!
   ```

The device will continuously:
- Monitor WiFi connection
- Check balance every 30 seconds
- Detect and notify balance changes
- Automatically reconnect if WiFi drops

## Customization

### Change Check Interval

To check balance more or less frequently:

```cpp
const unsigned long checkInterval = 60000;  // 60 seconds
const unsigned long checkInterval = 10000;  // 10 seconds
```

**Note**: Be respectful of API rate limits. 30 seconds is a safe default.

### Extract Additional Fields

To get more information from the API response:

```cpp
// Get UTXO value
const char* utxoStr = accountInfo["utxo"];
long long utxoLovelace = atoll(utxoStr);
float utxo = utxoLovelace / 1000000.0;

// Get rewards
const char* rewardsStr = accountInfo["rewards_available"];
long long rewardsLovelace = atoll(rewardsStr);
float rewards = rewardsLovelace / 1000000.0;

// Get delegation status
const char* poolId = accountInfo["delegated_pool"];
if (poolId != nullptr) {
    Serial.println("Delegated to pool: " + String(poolId));
}
```

### Use Mainnet Instead of Testnet

Change the API URL and stake address:

```cpp
const char* apiUrl = "https://api.koios.rest/api/v1/account_info";
String stakeAddress = "stake1..."; // Mainnet address (starts with stake1)
```

### Multiple Stake Addresses

To check multiple addresses at once:

```cpp
String jsonPayload = "{\"_stake_addresses\":[";
jsonPayload += "\"stake_test1...\",";
jsonPayload += "\"stake_test1...\"";
jsonPayload += "]}";
```

## Troubleshooting

### "JSON parsing failed" Error

- **Check Serial Monitor**: Look at the raw response to see what was returned
- **Buffer size**: Increase `DynamicJsonDocument doc(2048)` if response is large
- **Invalid JSON**: Verify API returned valid JSON
- **ArduinoJSON version**: Ensure you have a recent version installed

### HTTP Response Code: -1

- **WiFi not connected**: Check WiFi credentials and signal strength
- **Internet connection**: Verify your router has internet access
- **Firewall blocking**: Some networks block HTTPS requests
- **API server down**: Check if [preprod.koios.rest](https://preprod.koios.rest/) is accessible

### "Error in HTTP request" or Wrong Response Code

- **Wrong stake address**: Verify your stake address is correct
- **Testnet mismatch**: Ensure you're using Preprod addresses with Preprod API
- **API endpoint changed**: Check Koios documentation for updates
- **Rate limiting**: You may be making requests too frequently

### Balance Shows 0 or Wrong Value

- **Stake address format**: Ensure it starts with `stake_test1` (testnet) or `stake1` (mainnet)
- **Address not registered**: New addresses may not have balance data yet
- **Wrong network**: Make sure you're using Preprod API with Preprod addresses
- **Parsing error**: Check Serial Monitor for JSON parsing errors

### Balance Change Detection Not Working

- **Float comparison**: Floating point numbers may have precision issues
- **Check threshold**: Consider using a minimum change threshold:
  ```cpp
  float threshold = 0.000001; // Minimum change to detect
  if (abs(balance - previousBalance) > threshold) {
      // Balance changed
  }
  ```

## Next Steps

Once you've mastered fetching balances, try:

- **Display on TFT**: Show balance on a display (covered in next workshop step)
- **LED notifications**: Turn on LED when balance changes
- **Multiple addresses**: Monitor multiple stake addresses
- **Historical tracking**: Store balance history
- **Alerts**: Set up alerts for specific balance thresholds
- **Integration**: Combine with other Cardano data (tokens, NFTs, etc.)

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/fetch-wallet-balance)

## Additional Resources

- [Koios API Documentation](https://preprod.koios.rest/) - Full API reference
- [ArduinoJSON Documentation](https://arduinojson.org/) - JSON parsing guide
- [ESP32 HTTPClient Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient) - HTTP client docs
- [Cardano Addresses Guide](https://docs.cardano.org/cardano-testnet/tools/faucet) - Understanding Cardano addresses
- [Yoroi Wallet](https://yoroi-wallet.com/) - Get your stake address

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit WiFi credentials to version control** (Git)
- **Never commit stake addresses** (they're public but best practice)
- **Use environment variables or separate config files** for production code
- **HTTPS is automatic**: ESP32 HTTPClient handles SSL/TLS encryption
- **Stake addresses are public**: They don't need to be secret, but be mindful of privacy
