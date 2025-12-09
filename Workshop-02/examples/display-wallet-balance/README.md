# Wallet Balance Display Code

Display Cardano wallet balance on TFT screen with live updates and timestamp

## Overview

This sketch combines WiFi connectivity, API requests, JSON parsing, and TFT display control into a complete wallet monitoring system. It fetches your Cardano stake address balance from the Koios API every 60 seconds and displays it on a TFT screen with a live timestamp that updates every second. The display uses a clean design with white text on a blue background.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **Cheap Yellow Display (CYD)** with built-in TFT display
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- **TFT_eSPI library** (must be installed and configured for CYD)
- **ArduinoJSON library** (must be installed via Library Manager)
- Cardano stake address (Preprod Testnet)
- USB cable for programming and power

## How It Works

### Program Structure

This sketch combines multiple components:
- **WiFi connectivity**: Connects to your network
- **API requests**: Fetches balance from Koios API
- **JSON parsing**: Extracts balance data from API response
- **TFT display control**: Shows balance and timestamp on screen

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

4. **`#include <TFT_eSPI.h>`**
   - TFT display library for ESP32
   - **Must be installed** via Library Manager
   - **Must be configured** for CYD (replace User_Setup.h)

5. **`#include <SPI.h>`**
   - Serial Peripheral Interface library
   - Required for SPI communication with TFT display

#### Configuration Variables

6. **WiFi Credentials**
   ```cpp
   const char* ssid = "Your SSID";
   const char* password = "Your Password";
   ```
   - **Important**: Replace with your actual WiFi credentials before uploading!

7. **API Endpoint**
   ```cpp
   const char* apiUrl = "https://preprod.koios.rest/api/v1/account_info";
   ```
   - Koios API endpoint for fetching account information
   - Uses Preprod Testnet (for mainnet, use `https://api.koios.rest/api/v1/account_info`)

8. **Stake Address**
   ```cpp
   String stakeAddress = "stake_test1...";
   ```
   - **Important**: Replace with your actual Cardano stake address
   - Must be a Preprod Testnet address (starts with `stake_test1`)
   - Get your stake address from Yoroi wallet (Receive tab → Rewards section)

9. **Timing Variables**
   ```cpp
   unsigned long lastCheck = 0;
   const unsigned long checkInterval = 60000;  // 60 seconds
   unsigned long lastFetchTime = 0;
   unsigned long lastDisplayUpdate = 0;
   const unsigned long displayUpdateInterval = 1000;  // 1 second
   ```
   - Controls how often balance is checked (60 seconds)
   - Controls how often timestamp updates (1 second)
   - Uses non-blocking timing (doesn't freeze the device)

10. **Balance Storage**
    ```cpp
    float currentBalance = 0.0;
    ```
    - Stores current balance to detect changes
    - Used to compare with new balance values

#### Setup Function

11. **Serial Communication**
    ```cpp
    Serial.begin(115200);
    ```
    - Initializes serial communication at 115200 baud
    - **Note**: Set Serial Monitor to 115200 baud to view messages

12. **Display Initialization**
    ```cpp
    tft.init();
    tft.setRotation(1);        // Landscape orientation
    tft.invertDisplay(true);    // Invert colors for correct display
    tft.fillScreen(TFT_BLUE);  // Blue background
    ```
    - Powers on display and initializes ILI9341 driver
    - Sets landscape orientation (320×240 pixels)
    - Inverts colors (required for some CYD displays)
    - Fills screen with blue background

13. **Text Configuration**
    ```cpp
    tft.setTextColor(TFT_WHITE, TFT_BLUE);  // White text on blue
    tft.setTextSize(2);                     // Medium text size
    ```
    - Sets text color to white with blue background
    - Sets initial text size

14. **Startup Display**
    ```cpp
    tft.setCursor(10, 10);
    tft.println("Connecting...");
    ```
    - Shows "Connecting..." message while WiFi connects
    - Provides visual feedback during startup

15. **WiFi Connection**
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

16. **Initial Balance Fetch**
    ```cpp
    fetchStakeBalance();
    ```
    - Fetches balance immediately on startup
    - Provides instant feedback that the device is working

#### Loop Function

17. **WiFi Monitoring**
    ```cpp
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        // Wait for reconnection...
    }
    ```
    - Continuously checks WiFi connection
    - Automatically reconnects if connection is lost
    - Ensures device stays online

18. **Timed Balance Checks**
    ```cpp
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= checkInterval) {
        fetchStakeBalance();
        lastCheck = currentMillis;
    }
    ```
    - Uses `millis()` for non-blocking timing
    - Checks balance every 60 seconds
    - Doesn't freeze the device (unlike `delay()`)

19. **Timestamp Updates**
    ```cpp
    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        updateTimestamp();
        lastDisplayUpdate = currentMillis;
    }
    ```
    - Updates timestamp display every second
    - Provides live feedback on when balance was last fetched
    - Efficient - only updates timestamp area, not entire screen

#### Fetch Balance Function

20. **HTTP Client Setup**
    ```cpp
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    ```
    - Creates HTTP client object
    - Initializes with API URL
    - Sets content type header (required for JSON POST requests)

21. **JSON Payload Creation**
    ```cpp
    String jsonPayload = "{\"_stake_addresses\":[\"";
    jsonPayload += stakeAddress;
    jsonPayload += "\"]}";
    ```
    - Builds JSON payload step by step to avoid escaping issues
    - Koios API expects format: `{"_stake_addresses": ["stake_test1..."]}`
    - Can include multiple addresses in the array

22. **POST Request**
    ```cpp
    int httpResponseCode = http.POST(jsonPayload);
    ```
    - Sends POST request with JSON payload
    - Returns HTTP response code:
      - **200**: Success
      - **-1**: Connection error
      - **Other codes**: HTTP errors

23. **Response Parsing**
    ```cpp
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);
    ```
    - Creates JSON document with 2048 byte buffer
    - Parses JSON response string
    - Handles parsing errors gracefully

24. **Array Validation**
    ```cpp
    if (doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject accountInfo = doc[0];
    }
    ```
    - Verifies response is an array
    - Checks array has at least one element
    - Accesses first account info object

25. **Balance Extraction**
    ```cpp
    const char* balanceStr = accountInfo["total_balance"];
    long long balanceLovelace = atoll(balanceStr);
    ```
    - Koios returns balance as **string** (to handle large numbers)
    - **`atoll()`**: Converts string to `long long` integer
    - Necessary because Cardano balances can be very large

26. **Unit Conversion**
    ```cpp
    float balance = balanceLovelace / 1000000.0;
    ```
    - Converts from Lovelace to tADA (test ADA)
    - **1 tADA = 1,000,000 Lovelace**
    - Uses float for decimal precision

27. **Balance Change Detection**
    ```cpp
    if (balance != currentBalance) {
        if (balance > currentBalance) {
            Serial.println("✓ Balance increased!");
        } else {
            Serial.println("✓ Balance decreased!");
        }
        currentBalance = balance;
    }
    ```
    - Compares current balance with previous balance
    - Logs whether balance increased or decreased
    - Updates stored balance for next comparison

28. **Display Update Trigger**
    ```cpp
    lastFetchTime = millis();
    updateDisplay();
    ```
    - Records timestamp of successful fetch
    - Calls function to refresh entire display
    - Only updates if balance changed (efficient)

#### Display Update Function

29. **Full Screen Refresh**
    ```cpp
    tft.fillScreen(TFT_BLUE);
    ```
    - Fills entire screen with blue background
    - Clears any previous content
    - Creates consistent appearance

30. **Title Display**
    ```cpp
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(10, 10);
    tft.println("Wallet Balance");
    ```
    - Displays "Wallet Balance" title at top
    - White text, size 2 (medium)
    - Positioned at (10, 10) - top-left area

31. **Balance Display**
    ```cpp
    tft.setTextSize(4);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(10, 50);
    tft.print(String(currentBalance, 2));  // 2 decimal places
    ```
    - Displays balance amount in large text
    - Size 4 (large, prominent)
    - Formatted to 2 decimal places
    - Positioned below title

32. **Unit Label**
    ```cpp
    tft.setTextSize(2);
    tft.println(" ADA");
    ```
    - Displays "ADA" unit label
    - Size 2 (medium)
    - Appears right after balance amount

33. **Initial Timestamp**
    ```cpp
    updateTimestamp();
    ```
    - Calls timestamp update function
    - Displays initial "Updated 0s ago" message

#### Timestamp Update Function

34. **Time Calculation**
    ```cpp
    unsigned long secondsAgo = (millis() - lastFetchTime) / 1000;
    ```
    - Calculates seconds elapsed since last successful fetch
    - Divides milliseconds by 1000 to get seconds
    - Updates every second for live counter

35. **Efficient Redraw**
    ```cpp
    tft.fillRect(10, 220, 200, 10, TFT_BLUE);
    ```
    - Clears only the timestamp area (200×10 pixels)
    - Doesn't redraw entire screen (prevents flickering)
    - Positioned at bottom-left (y=220)

36. **Timestamp Display**
    ```cpp
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(10, 220);
    tft.print("Updated ");
    tft.print(secondsAgo);
    tft.println("s ago");
    ```
    - Displays "Updated Xs ago" message
    - Size 1 (small, unobtrusive)
    - Shows live counter that updates every second

### Display Layout

The screen layout (320×240 pixels):

```
┌─────────────────────────────────┐
│ Wallet Balance          (10,10) │
│                                 │
│ 123.45 ADA            (10,50)  │
│                                 │
│                                 │
│                                 │
│                                 │
│                                 │
│                                 │
│ Updated 5s ago        (10,220) │
└─────────────────────────────────┘
```

**Element Positions:**
- **Title**: (10, 10) - "Wallet Balance" in size 2
- **Balance**: (10, 50) - Balance amount in size 4
- **Unit**: Right after balance - "ADA" in size 2
- **Timestamp**: (10, 220) - "Updated Xs ago" in size 1

## Installation

### Step 1: Install Required Libraries

1. **TFT_eSPI Library**:
   - Open Arduino IDE
   - Go to **Sketch → Include Library → Manage Libraries**
   - Search for **"TFT_eSPI"**
   - Install **"TFT_eSPI" by Bodmer**

2. **ArduinoJSON Library**:
   - In Library Manager, search for **"ArduinoJson"**
   - Install **"ArduinoJson" by Benoit Blanchon**

### Step 2: Configure TFT_eSPI for CYD

**CRITICAL**: The TFT_eSPI library must be configured specifically for the CYD:

1. Download the CYD-specific `User_Setup.h` file from:
   [ESP32-Cheap-Yellow-Display Repository](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/DisplayConfig/User_Setup.h)

2. Locate your TFT_eSPI library folder:
   - **Windows**: `Documents\Arduino\libraries\TFT_eSPI\`
   - **Mac**: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - **Linux**: `~/Arduino/libraries/TFT_eSPI/`

3. **Replace** the existing `User_Setup.h` file with the downloaded CYD version

4. **Restart Arduino IDE** for changes to take effect

**⚠️ Warning**: If you skip this configuration step, your display will NOT work correctly!

### Step 3: Configure the Code

**Before uploading**, update these lines:

```cpp
const char* ssid = "Your SSID";        // Change to your WiFi name
const char* password = "Your Password"; // Change to your WiFi password
String stakeAddress = "stake_test1..."; // Change to your stake address
```

### Step 4: Upload Code

1. Copy the code from `tft-display-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Watch your display!

## Usage

After uploading, the display will show:

1. **Startup Phase**:
   - "Connecting..." message appears
   - WiFi connection process
   - "Connected!" message

2. **Main Display**:
   - **Title**: "Wallet Balance" at top
   - **Balance**: Large display of your ADA balance (e.g., "123.45 ADA")
   - **Timestamp**: "Updated Xs ago" at bottom (updates every second)

3. **Updates**:
   - Balance fetches every 60 seconds
   - Timestamp updates every second
   - Display refreshes when balance changes

The device will continuously:
- Monitor WiFi connection
- Fetch balance every 60 seconds
- Update timestamp every second
- Automatically reconnect if WiFi drops

## Customization

### Change Update Intervals

To fetch balance more or less frequently:

```cpp
const unsigned long checkInterval = 30000;  // 30 seconds
const unsigned long checkInterval = 120000; // 2 minutes
```

**Note**: Be respectful of API rate limits. 60 seconds is a safe default.

### Change Display Colors

To use different colors:

```cpp
tft.fillScreen(TFT_BLACK);  // Black background
tft.setTextColor(TFT_YELLOW, TFT_BLACK);  // Yellow text on black
```

Available colors: `TFT_BLACK`, `TFT_WHITE`, `TFT_RED`, `TFT_GREEN`, `TFT_BLUE`, `TFT_YELLOW`, `TFT_CYAN`, `TFT_MAGENTA`

### Change Text Sizes

To adjust text sizes:

```cpp
// Larger balance
tft.setTextSize(5);  // Instead of 4

// Smaller title
tft.setTextSize(1);  // Instead of 2
```

### Change Display Layout

To reposition elements:

```cpp
// Move title to center
tft.setCursor(100, 10);  // Instead of (10, 10)

// Move balance to center
tft.setCursor(80, 100);  // Instead of (10, 50)
```

### Add More Information

To display additional data:

```cpp
// Show stake address
tft.setTextSize(1);
tft.setCursor(10, 180);
tft.println("Address: " + String(stakeAddress.substring(0, 20) + "..."));

// Show WiFi status
tft.setCursor(10, 200);
tft.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"));
```

### Use Mainnet Instead of Testnet

Change the API URL and stake address:

```cpp
const char* apiUrl = "https://api.koios.rest/api/v1/account_info";
String stakeAddress = "stake1..."; // Mainnet address (starts with stake1)
```

## Troubleshooting

### Display Shows Nothing

- **Check User_Setup.h**: Ensure you replaced it with the CYD version
- **Check connections**: Verify CYD is properly connected to ESP32
- **Check power**: Ensure CYD is receiving power (USB cable)
- **Restart Arduino IDE**: After changing User_Setup.h, IDE must restart

### Colors Are Wrong

- **Try inverting**: Change `tft.invertDisplay(true)` to `tft.invertDisplay(false)`
- **Check User_Setup.h**: Ensure CYD-specific configuration is correct

### Balance Shows 0.00

- **Check stake address**: Verify your stake address is correct
- **Check WiFi**: Ensure device is connected to internet
- **Check API**: Verify Koios API is accessible
- **Check Serial Monitor**: Look for error messages

### Timestamp Not Updating

- **Check loop function**: Ensure `updateTimestamp()` is being called
- **Check timing**: Verify `displayUpdateInterval` is set correctly
- **Check lastFetchTime**: Ensure it's being set when balance is fetched

### Display Flickers

- **Reduce updates**: Increase `displayUpdateInterval` to update less frequently
- **Partial updates**: Only update changed areas (like timestamp function does)
- **Check power**: Ensure stable power supply

### WiFi Connection Issues

- **Wrong credentials**: Double-check SSID and password
- **Out of range**: Move ESP32 closer to router
- **5GHz network**: ESP32 only supports 2.4GHz WiFi

### API Request Fails

- **Check Serial Monitor**: Look for HTTP error codes
- **Check stake address**: Verify format is correct
- **Check API endpoint**: Ensure URL is correct
- **Check internet**: Verify router has internet access

## Next Steps

Once you've mastered this example, try:

- **Multiple addresses**: Display multiple stake addresses
- **Historical tracking**: Store and display balance history
- **Alerts**: Show alerts when balance changes significantly
- **Charts**: Display balance trends over time
- **Additional data**: Show rewards, delegation status, etc.
- **Custom graphics**: Add icons, logos, or custom designs

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/display-data)

## Additional Resources

- [TFT_eSPI Library GitHub](https://github.com/Bodmer/TFT_eSPI) - Official repository with documentation
- [Koios API Documentation](https://preprod.koios.rest/) - Full API reference
- [ArduinoJSON Documentation](https://arduinojson.org/) - JSON parsing guide
- [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display) - CYD-specific resources
- [ESP32 HTTPClient Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient) - HTTP client docs

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit WiFi credentials to version control** (Git)
- **Never commit stake addresses** to version control (they're public but best practice)
- **Use environment variables or separate config files** for production code
- **HTTPS is automatic**: ESP32 HTTPClient handles SSL/TLS encryption
- **Stake addresses are public**: They don't need to be secret, but be mindful of privacy
