# Blockfrost API Code

Fetch Cardano stake address balance from Blockfrost API using GET requests

## Overview

This sketch demonstrates how to fetch your Cardano stake address balance from the Blockfrost API using a GET request. Unlike Koios (which uses POST), Blockfrost uses simpler GET requests with the stake address in the URL path. The code connects to WiFi, sends your API key in a header, parses the JSON response, and displays your balance in the Serial Monitor.

## Requirements

- ESP32 or ESP8266 based microcontroller
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- **ArduinoJSON library** (must be installed via Library Manager)
- **Blockfrost API key** (free tier available at blockfrost.io)
- Cardano stake address (Preprod Testnet)
- USB cable for programming and power

## How It Works

### Code Structure

The sketch is organized into several key functions:
- **`setup()`**: Initializes serial communication and connects to WiFi
- **`loop()`**: Monitors WiFi connection and calls balance fetch function every 30 seconds
- **`fetchStakeBalance()`**: Builds URL, sends GET request with API key, and parses response

### Understanding the Code

#### Libraries

1. **`#include <WiFi.h>`**
   - Provides WiFi connectivity functions
   - Part of ESP32 Arduino core (no installation needed)

2. **`#include <HTTPClient.h>`**
   - Provides HTTP/HTTPS client functionality
   - Part of ESP32 Arduino core (no installation needed)
   - Handles GET requests (simpler than POST)

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
   const char* apiUrl = "https://cardano-preprod.blockfrost.io/api/v0/accounts/";
   ```
   - Blockfrost API base URL for Preprod Testnet
   - Note the trailing slash - stake address will be appended
   - For mainnet, use: `https://cardano-mainnet.blockfrost.io/api/v0/accounts/`

6. **API Key**
   ```cpp
   const char* apiKey = "your-blockfrost-api-key";
   ```
   - **Important**: Replace with your actual Blockfrost API key
   - Get a free API key at [blockfrost.io](https://blockfrost.io/)
   - Create a new project and select Preprod Testnet
   - Copy the API key from your project dashboard

7. **Stake Address**
   ```cpp
   String stakeAddress = "stake_test1...";
   ```
   - **Important**: Replace with your actual Cardano stake address
   - Must be a Preprod Testnet address (starts with `stake_test1`)
   - Get your stake address from Yoroi wallet (Receive tab → Rewards section)

8. **Timing Variables**
   ```cpp
   unsigned long lastCheck = 0;
   const unsigned long checkInterval = 30000; // 30 seconds
   ```
   - Controls how often balance is checked
   - Uses non-blocking timing (doesn't freeze the device)
   - Adjust interval based on your needs (but respect API rate limits)

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
    - Displays connection status

#### Loop Function

11. **WiFi Monitoring**
    ```cpp
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
    }
    ```
    - Continuously checks WiFi connection
    - Automatically reconnects if connection is lost
    - Ensures device stays online

12. **Timed Balance Checks**
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

13. **URL Construction**
    ```cpp
    String fullUrl = apiUrl + stakeAddress;
    ```
    - Builds full URL by appending stake address to base URL
    - Example: `https://cardano-preprod.blockfrost.io/api/v0/accounts/stake_test1...`
    - Much simpler than Koios (no JSON payload needed!)

14. **HTTP Client Setup**
    ```cpp
    HTTPClient http;
    http.begin(fullUrl);
    http.addHeader("project_id", apiKey);
    ```
    - Creates HTTP client object
    - Initializes with full URL
    - **Important**: Blockfrost requires API key in `project_id` header
    - This is how Blockfrost authenticates your requests

15. **GET Request**
    ```cpp
    int httpResponseCode = http.GET();
    ```
    - Sends GET request (simpler than POST!)
    - No payload needed - stake address is in URL
    - Returns HTTP response code:
      - **200**: Success
      - **401**: Unauthorized (wrong API key)
      - **404**: Not Found (wrong stake address)
      - **-1**: Connection error

16. **Response Handling**
    ```cpp
    if (httpResponseCode > 0) {
        String response = http.getString();
        // Parse JSON...
    }
    ```
    - Checks if request was successful
    - Retrieves response body as string
    - Response contains account information object (not array like Koios)

17. **JSON Parsing**
    ```cpp
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    ```
    - Creates JSON document with 1024 byte buffer
    - Parses JSON response string
    - Blockfrost response is smaller than Koios (single object vs array)

18. **Balance Extraction**
    ```cpp
    const char* balanceStr = doc["controlled_amount"];
    long long balanceLovelace = atoll(balanceStr);
    ```
    - Blockfrost returns balance as **string** (to handle large numbers)
    - **`controlled_amount`**: Total balance including delegated amount and rewards
    - **`atoll()`**: Converts string to `long long` integer
    - Necessary because Cardano balances can be very large

19. **Unit Conversion**
    ```cpp
    float balance = balanceLovelace / 1000000.0;
    ```
    - Converts from Lovelace to tADA (test ADA)
    - **1 tADA = 1,000,000 Lovelace**
    - Uses float for decimal precision

20. **Display Balance**
    ```cpp
    Serial.println("Stake Address: " + String(doc["stake_address"]));
    Serial.println("Total Balance: " + String(balance, 6) + " tADA");
    ```
    - Prints stake address and balance
    - Balance displayed with 6 decimal places
    - Easy to read format

### API Request Format

**Blockfrost uses GET requests** (simpler than Koios POST):

**Request:**
```
GET https://cardano-preprod.blockfrost.io/api/v0/accounts/stake_test1...
Headers:
  project_id: your-api-key
```

**Response:**
```json
{
  "stake_address": "stake_test1ux3g2c9dx2nhhehyrezyxpkstartcqmu9hk63qgfkccw5rqttygt7",
  "active": true,
  "active_epoch": 412,
  "controlled_amount": "619154618165",
  "rewards_sum": "319154618165",
  "withdrawals_sum": "12125369253",
  "reserves_sum": "319154618165",
  "treasury_sum": "12000000",
  "withdrawable_amount": "319154618165",
  "pool_id": "pool1pu5jlj4q9w9jlxeu370a3c9myx47md5j5m2str0naunn2q3lkdy",
  "drep_id": "drep15cfxz9exyn5rx0807zvxfrvslrjqfchrd4d47kv9e0f46uedqtc"
}
```

**Key Response Fields:**
- **`stake_address`**: Your stake address
- **`active`**: Whether the stake address is currently active
- **`controlled_amount`**: Total balance in Lovelace (as string) - this is what we use
- **`rewards_sum`**: Total rewards earned in Lovelace
- **`withdrawable_amount`**: Amount available to withdraw in Lovelace
- **`pool_id`**: Stake pool ID if delegated (null if not)
- **`drep_id`**: DRep ID for governance voting

## Installation

### Step 1: Get Blockfrost API Key

1. Go to [blockfrost.io](https://blockfrost.io/)
2. Click **"Sign Up"** or **"Log In"**
3. Create a new project:
   - Click **"Create Project"**
   - Select **"Preprod"** (for testnet) or **"Mainnet"** (for production)
   - Give your project a name
   - Click **"Create"**
4. Copy your API key from the project dashboard
5. **Important**: Keep your API key secure and don't share it publicly

### Step 2: Install ArduinoJSON Library

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. In the search box, type **ArduinoJson**
4. Find **"ArduinoJson" by Benoit Blanchon** (the one with millions of downloads)
5. Click **Install**
6. Wait for installation to complete

### Step 3: Get Your Stake Address

1. Open your Yoroi wallet extension
2. Make sure you're on the **Preprod Testnet** (orange banner at the top)
3. Go to the **"Wallet"** section
4. Click on the **"Receive"** tab
5. Copy your stake address from the **rewards section** (starts with `stake_test1...`)

### Step 4: Configure the Code

**Before uploading**, update these lines:

```cpp
const char* ssid = "Your SSID";                    // Change to your WiFi name
const char* password = "Your Password";           // Change to your WiFi password
const char* apiKey = "your-blockfrost-api-key";   // Change to your Blockfrost API key
String stakeAddress = "stake_test1...";           // Change to your stake address
```

### Step 5: Upload Code

1. Copy the code from `blockfrost-code.ino`
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
   ```

2. **Balance Updates** (every 30 seconds):
   ```
   Stake Address: stake_test1...
   Total Balance: 619.154618 tADA
   ```

The device will continuously:
- Monitor WiFi connection
- Check balance every 30 seconds
- Automatically reconnect if WiFi drops

## Blockfrost vs Koios

### Advantages of Blockfrost

- **Simpler requests**: GET instead of POST (no JSON payload needed)
- **Single object response**: Easier to parse than Koios array
- **Official SDK available**: For PlatformIO users
- **Well-documented**: Comprehensive API documentation

### Advantages of Koios

- **No API key required**: Completely free and open
- **No rate limits**: More generous usage policies
- **Community-driven**: Open source project

### When to Use Each

- **Use Blockfrost** if:
  - You need a simpler GET-based API
  - You want official SDK support
  - You don't mind signing up for an API key
  - You need higher rate limits (paid tiers)

- **Use Koios** if:
  - You want to avoid API key management
  - You prefer open source solutions
  - You want no sign-up required
  - You're building open source projects

## Customization

### Change Check Interval

To check balance more or less frequently:

```cpp
const unsigned long checkInterval = 60000;  // 60 seconds
const unsigned long checkInterval = 10000;  // 10 seconds
```

**Note**: Blockfrost free tier has rate limits. Check [blockfrost.io/pricing](https://blockfrost.io/pricing) for details.

### Extract Additional Fields

To get more information from the API response:

```cpp
// Get rewards
const char* rewardsStr = doc["rewards_sum"];
long long rewardsLovelace = atoll(rewardsStr);
float rewards = rewardsLovelace / 1000000.0;

// Get withdrawable amount
const char* withdrawableStr = doc["withdrawable_amount"];
long long withdrawableLovelace = atoll(withdrawableStr);
float withdrawable = withdrawableLovelace / 1000000.0;

// Check delegation status
const char* poolId = doc["pool_id"];
if (poolId != nullptr) {
    Serial.println("Delegated to pool: " + String(poolId));
}

// Check if active
bool active = doc["active"];
Serial.println("Active: " + String(active ? "Yes" : "No"));
```

### Use Mainnet Instead of Testnet

Change the API URL and API key:

```cpp
const char* apiUrl = "https://cardano-mainnet.blockfrost.io/api/v0/accounts/";
String stakeAddress = "stake1..."; // Mainnet address (starts with stake1)
// Use mainnet API key from blockfrost.io
```

### Increase JSON Buffer Size

If you need to parse larger responses:

```cpp
DynamicJsonDocument doc(2048); // Increase from 1024 to 2048
```

## Troubleshooting

### HTTP Response Code: 401 (Unauthorized)

- **Wrong API key**: Double-check your API key is correct
- **Wrong network**: Ensure API key matches network (Preprod vs Mainnet)
- **Expired key**: Check if your API key is still valid in Blockfrost dashboard
- **Rate limit exceeded**: Free tier has limits - wait or upgrade

### HTTP Response Code: 404 (Not Found)

- **Wrong stake address**: Verify your stake address is correct
- **Testnet mismatch**: Ensure you're using Preprod addresses with Preprod API
- **Address not registered**: New addresses may not have data yet

### HTTP Response Code: -1

- **WiFi not connected**: Check WiFi credentials and signal strength
- **Internet connection**: Verify your router has internet access
- **Firewall blocking**: Some networks block HTTPS requests
- **API server down**: Check if [blockfrost.io](https://blockfrost.io/) is accessible

### "JSON parsing failed" Error

- **Check Serial Monitor**: Look at the raw response to see what was returned
- **Buffer size**: Increase `DynamicJsonDocument doc(1024)` if response is large
- **Invalid JSON**: Verify API returned valid JSON
- **ArduinoJSON version**: Ensure you have a recent version installed

### Balance Shows 0 or Wrong Value

- **Stake address format**: Ensure it starts with `stake_test1` (testnet) or `stake1` (mainnet)
- **Address not registered**: New addresses may not have balance data yet
- **Wrong network**: Make sure you're using Preprod API with Preprod addresses
- **Parsing error**: Check Serial Monitor for JSON parsing errors

### Rate Limiting

Blockfrost free tier has rate limits:
- **10 requests/second** for free tier
- **500 requests/day** for free tier

If you hit rate limits:
- Increase `checkInterval` to make requests less frequently
- Upgrade to a paid tier for higher limits
- Consider using Koios for unlimited requests

## Next Steps

Once you've mastered Blockfrost, try:

- **Display on TFT**: Show balance on a display
- **Multiple addresses**: Monitor multiple stake addresses
- **Historical tracking**: Store balance history
- **Alerts**: Set up alerts for specific balance thresholds
- **Official SDK**: Use Blockfrost Arduino SDK for PlatformIO
- **Integration**: Combine with other Blockfrost endpoints

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/fetch-wallet-balance)

## Additional Resources

- [Blockfrost Documentation](https://docs.blockfrost.io/) - Complete API reference
- [Blockfrost Dashboard](https://blockfrost.io/) - Get API keys and manage projects
- [Blockfrost Pricing](https://blockfrost.io/pricing) - Rate limits and pricing tiers
- [Blockfrost Arduino SDK](https://github.com/blockfrost/blockfrost-arduino) - Official SDK for PlatformIO
- [ArduinoJSON Documentation](https://arduinojson.org/) - JSON parsing guide
- [ESP32 HTTPClient Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient) - HTTP client docs

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit API keys to version control** (Git)
- **Never commit WiFi credentials** to version control
- **Use environment variables or separate config files** for production code
- **HTTPS is automatic**: ESP32 HTTPClient handles SSL/TLS encryption
- **API keys are sensitive**: Keep them secure and rotate regularly
- **Free tier limits**: Be aware of rate limits to avoid service interruption

