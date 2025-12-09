# Relay Control with Blockchain Integration

Control a relay based on Cardano wallet balance changes - Connect blockchain events to physical hardware

## Overview

This sketch demonstrates how to connect blockchain events to physical hardware using a relay. It monitors your Cardano wallet balance using the Koios API and controls a relay based on balance changes. When your wallet balance increases (you receive a transaction), the relay activates, turning on your connected device. When the balance decreases (you send a transaction), the relay deactivates, turning off your device.

## Requirements

- **ESP32-C3** microcontroller (or compatible ESP32 board)
- **Relay Module** (single-channel relay module recommended)
- Arduino IDE with ESP32 board support installed
- WiFi network with SSID and password
- **ArduinoJSON library** (must be installed via Library Manager)
- Cardano wallet address (Preprod Testnet)
- USB cable for programming and power
- **Optional**: Device to control (light, appliance, etc.)

## How It Works

### Program Structure

This sketch combines multiple components:
- **WiFi connectivity**: Connects to your network
- **API requests**: Fetches balance from Koios API
- **JSON parsing**: Extracts balance data from API response
- **Balance monitoring**: Detects balance changes
- **Relay control**: Activates/deactivates relay based on balance changes

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
   const char* apiUrl = "https://preprod.koios.rest/api/v1/address_info";
   ```
   - Koios API endpoint for fetching address information
   - Uses Preprod Testnet (for mainnet, use `https://api.koios.rest/api/v1/address_info`)
   - **Note**: Uses `/address_info` endpoint (not `/account_info` like stake address examples)

6. **Wallet Address**
   ```cpp
   String walletAddress = "addr_test1...";
   ```
   - **Important**: Replace with your actual Cardano wallet address
   - Must be a Preprod Testnet address (starts with `addr_test1`)
   - This is your **payment address**, not stake address
   - Get your wallet address from Yoroi wallet (Receive tab)

7. **Relay Pin**
   ```cpp
   const int relayPin = 4;
   ```
   - GPIO pin connected to relay module IN pin
   - **Important**: Change this to match your wiring
   - Common pins: GPIO 2, GPIO 4, GPIO 5

8. **Timing Variables**
   ```cpp
   unsigned long lastCheck = 0;
   const unsigned long checkInterval = 30000;  // 30 seconds
   ```
   - Controls how often balance is checked (30 seconds)
   - Uses non-blocking timing (doesn't freeze the device)

9. **Balance Tracking**
   ```cpp
   float previousBalance = 0;
   bool lightState = false;
   ```
   - Stores previous balance to detect changes
   - Tracks current relay/light state

#### Setup Function

10. **Serial Communication**
    ```cpp
    Serial.begin(115200);
    ```
    - Initializes serial communication at 115200 baud
    - **Note**: Set Serial Monitor to 115200 baud to view messages

11. **Relay Pin Configuration**
    ```cpp
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);  // Start with relay off
    ```
    - Configures relay pin as OUTPUT
    - Sets initial state to OFF (LOW for active LOW relays)
    - **Note**: Some relays are active HIGH - adjust accordingly

12. **WiFi Connection**
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

13. **Initial Balance Check**
    ```cpp
    fetchWalletBalance();
    ```
    - Fetches balance immediately on startup
    - Establishes baseline for change detection

#### Loop Function

14. **WiFi Monitoring**
    ```cpp
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        // Wait for reconnection...
    }
    ```
    - Continuously checks WiFi connection
    - Automatically reconnects if connection is lost
    - Ensures device stays online

15. **Timed Balance Checks**
    ```cpp
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= checkInterval) {
        fetchWalletBalance();
        lastCheck = currentMillis;
    }
    ```
    - Uses `millis()` for non-blocking timing
    - Checks balance every 30 seconds
    - Doesn't freeze the device (unlike `delay()`)

#### Fetch Balance Function

16. **HTTP Client Setup**
    ```cpp
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    ```
    - Creates HTTP client object
    - Initializes with API URL
    - Sets content type header (required for JSON POST requests)

17. **JSON Payload Creation**
    ```cpp
    String jsonPayload = "{\"_addresses\":[\"" + walletAddress + "\"]}";
    ```
    - Builds JSON payload with wallet address
    - Koios API expects format: `{"_addresses": ["addr_test1..."]}`
    - Uses payment address (not stake address)

18. **POST Request**
    ```cpp
    int httpResponseCode = http.POST(jsonPayload);
    ```
    - Sends POST request with JSON payload
    - Returns HTTP response code:
      - **200**: Success
      - **-1**: Connection error
      - **Other codes**: HTTP errors

19. **Response Parsing**
    ```cpp
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);
    ```
    - Creates JSON document with 2048 byte buffer
    - Parses JSON response string
    - Handles parsing errors gracefully

20. **Array Validation**
    ```cpp
    if (!error && doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject addressInfo = doc[0];
    }
    ```
    - Verifies response is an array
    - Checks array has at least one element
    - Accesses first address info object

21. **Balance Extraction**
    ```cpp
    float balance = addressInfo["balance"] | 0.0;
    balance = balance / 1000000;  // Convert from Lovelace to ADA
    ```
    - Extracts balance from response
    - Uses default value (0.0) if balance field missing
    - Converts from Lovelace to ADA (1 ADA = 1,000,000 Lovelace)

22. **Change Detection**
    ```cpp
    if (balance > previousBalance) {
        Serial.println("New transaction detected! Turning on light...");
        turnOnLight();  // Activate relay
    } else if (balance < previousBalance) {
        Serial.println("Balance decreased. Turning off light...");
        turnOffLight();  // Deactivate relay
    }
    ```
    - Compares current balance with previous balance
    - **Balance increased**: Calls `turnOnLight()` to activate relay
    - **Balance decreased**: Calls `turnOffLight()` to deactivate relay
    - Logs actions to Serial Monitor

23. **Balance Update**
    ```cpp
    previousBalance = balance;
    ```
    - Updates stored balance for next comparison
    - Enables change detection on next check

#### Relay Control Functions

24. **Turn On Light**
    ```cpp
    void turnOnLight() {
        digitalWrite(relayPin, HIGH);  // Activate relay
        lightState = true;
        Serial.println("Light is ON");
    }
    ```
    - Sets relay pin to HIGH (for most relay modules)
    - Activates the relay (you should hear a click)
    - Updates state variable
    - Prints confirmation message
    - **Note**: Some relays are active LOW - adjust if needed

25. **Turn Off Light**
    ```cpp
    void turnOffLight() {
        digitalWrite(relayPin, LOW);  // Deactivate relay
        lightState = false;
        Serial.println("Light is OFF");
    }
    ```
    - Sets relay pin to LOW (for most relay modules)
    - Deactivates the relay (you should hear a click)
    - Updates state variable
    - Prints confirmation message

### Balance Monitoring Flow

The balance monitoring works in this sequence:

1. **Startup**: Connects to WiFi and performs initial balance check
2. **Every 30 seconds**: `loop()` calls `fetchWalletBalance()`
3. **API Request**: Sends POST request to Koios API with wallet address
4. **Response Parsing**: Extracts current balance from JSON response
5. **Balance Comparison**: Compares current balance with previous balance
6. **Action Trigger**:
   - **Balance increased** → `turnOnLight()` → Relay activates
   - **Balance decreased** → `turnOffLight()` → Relay deactivates
7. **State Update**: Updates `previousBalance` for next comparison
8. **Repeat**: Cycle continues every 30 seconds

### Non-Blocking Timing

The sketch uses `millis()` instead of `delay()` for timing:

- **`millis()`**: Returns milliseconds since device boot (non-blocking)
- **`delay()`**: Pauses execution (blocking - freezes device)
- **Non-blocking advantage**: Device remains responsive during delays
- **Allows**: WiFi monitoring, multiple timers, complex logic

## Installation

### Step 1: Install ArduinoJSON Library

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries** (or `Ctrl+Shift+I` / `Cmd+Shift+I`)
3. In the search box, type **ArduinoJson**
4. Find **"ArduinoJson" by Benoit Blanchon** (the one with millions of downloads)
5. Click **Install**
6. Wait for installation to complete

### Step 2: Wire Your Relay Module

Connect your relay module to the ESP32-C3:

1. **VCC** → Connect to **3.3V** or **5V** (check your relay module specifications)
2. **GND** → Connect to **GND** (ground)
3. **IN** (or SIG) → Connect to **GPIO 4** (or any available GPIO pin)

**Important**: Note which GPIO pin you connected the IN pin to - you'll need to update the code!

### Step 3: Get Your Wallet Address

1. Open your Yoroi wallet extension
2. Make sure you're on the **Preprod Testnet** (orange banner at the top)
3. Go to the **"Wallet"** section
4. Click on the **"Receive"** tab
5. Copy your **payment address** (starts with `addr_test1...`)
   - **Note**: This is different from your stake address!

### Step 4: Configure the Code

**Before uploading**, update these lines:

```cpp
const char* ssid = "Your SSID";                    // Change to your WiFi name
const char* password = "Your Password";           // Change to your WiFi password
String walletAddress = "addr_test1...";           // Change to your wallet address
const int relayPin = 4;                            // Change to match your wiring
```

### Step 5: Upload Code

1. Copy the code from `relay-code.ino`
2. Paste it into a new sketch in Arduino IDE
3. Select your board: **Tools → Board → ESP32 → ESP32C3 Dev Module**
4. Select the correct port: **Tools → Port → [Your Port]**
5. Click the Upload button (→ arrow icon)
6. Open Serial Monitor: **Tools → Serial Monitor** (or `Ctrl+Shift+M` / `Cmd+Shift+M`)
7. Set baud rate to **115200** in the Serial Monitor
8. Watch for balance changes!

## Usage

After uploading, the device will:

1. **Connect to WiFi**: Shows "Connecting to WiFi..." then "Connected to WiFi"
2. **Initial Balance Check**: Fetches balance on startup
3. **Monitor Balance**: Checks balance every 30 seconds
4. **React to Changes**:
   - **Receive transaction** → Relay activates → Light turns ON
   - **Send transaction** → Relay deactivates → Light turns OFF

**Serial Monitor Output**:
```
Connecting to WiFi...
Connected to WiFi
New transaction detected! Turning on light...
Light is ON
Balance decreased. Turning off light...
Light is OFF
```

## Testing

### Test with PingPong Wallet

Use the [PingPong Wallet](https://pingpongwallet.com/) to send test transactions:

1. Send a small amount (e.g., 1 tADA) to your wallet address
2. Wait up to 30 seconds (or until next check)
3. Relay should activate and light should turn ON
4. Send another transaction (or spend from your wallet)
5. Relay should deactivate and light should turn OFF

### What You Should See/Hear

When a transaction is detected:
- ✅ **Clicking sound** (relay switching)
- ✅ **Status LED** on relay module lights up (if present)
- ✅ **Serial Monitor** shows "New transaction detected! Turning on light..."
- ✅ **Connected device** turns on (if wired)

## Customization

### Only Trigger on Balance Increases

To only activate relay when receiving funds (ignore outgoing):

```cpp
if (balance > previousBalance) {
    turnOnLight();  // Only turn on when receiving
}
// Remove the else if for balance decreases
```

### Add Threshold Check

To only activate if balance exceeds a certain amount:

```cpp
const float threshold = 10.0;  // Minimum balance in ADA

if (balance > previousBalance && balance >= threshold) {
    turnOnLight();
}
```

### Timed Activation

To turn relay on for a specific duration:

```cpp
unsigned long relayOnTime = 0;
const unsigned long relayDuration = 60000;  // 60 seconds

if (balance > previousBalance) {
    turnOnLight();
    relayOnTime = millis();
}

// In loop()
if (lightState && (millis() - relayOnTime >= relayDuration)) {
    turnOffLight();
}
```

### Change Check Interval

To check balance more or less frequently:

```cpp
const unsigned long checkInterval = 10000;  // 10 seconds (faster)
const unsigned long checkInterval = 60000;  // 60 seconds (slower)
```

**Note**: Be respectful of API rate limits. 30 seconds is a safe default.

### Use Mainnet Instead of Testnet

Change the API URL and wallet address:

```cpp
const char* apiUrl = "https://api.koios.rest/api/v1/address_info";
String walletAddress = "addr1..."; // Mainnet address (starts with addr1)
```

### Multiple Relays

To control multiple relays for different events:

```cpp
const int relayPin1 = 4;  // For balance increases
const int relayPin2 = 5;  // For balance decreases

if (balance > previousBalance) {
    digitalWrite(relayPin1, HIGH);  // Turn on first relay
} else if (balance < previousBalance) {
    digitalWrite(relayPin2, HIGH);  // Turn on second relay
}
```

## Troubleshooting

### Relay Doesn't Activate

- **Check wiring**: Verify VCC, GND, and IN connections
- **Check pin number**: Ensure `relayPin` matches your wiring
- **Try reversing logic**: Change HIGH to LOW and LOW to HIGH (some relays are active HIGH)
- **Check power**: Verify relay module is receiving power (VCC)
- **Check Serial Monitor**: Look for "New transaction detected!" messages

### Balance Changes Not Detected

- **Check wallet address**: Verify format is correct (`addr_test1...`)
- **Check API endpoint**: Ensure you're using Preprod API with Preprod address
- **Check Serial Monitor**: Look for API errors or parsing errors
- **Wait for check interval**: Balance is checked every 30 seconds
- **Test with PingPong**: Send a test transaction to verify

### WiFi Connection Issues

- **Wrong credentials**: Double-check SSID and password
- **Out of range**: Move ESP32 closer to router
- **5GHz network**: ESP32 only supports 2.4GHz WiFi
- **Check Serial Monitor**: Look for connection errors

### API Request Fails

- **Check Serial Monitor**: Look for HTTP error codes
- **Check wallet address**: Verify format is correct
- **Check API endpoint**: Ensure URL is correct
- **Check internet**: Verify router has internet access
- **API server down**: Check if [preprod.koios.rest](https://preprod.koios.rest/) is accessible

### Relay Activates But Device Doesn't Work

- **High-voltage wiring**: Check COM and NO terminal connections
- **Power source**: Verify device is receiving power
- **Relay rating**: Ensure relay can handle your device's current/voltage
- **Safety**: Double-check high-voltage connections (if applicable)

## Safety Notes

⚠️ **CRITICAL Safety Considerations:**

- **High voltage warning**: Working with 110V/220V can be dangerous
- **Start with low voltage**: Test with LEDs before using high-voltage devices
- **Proper wiring**: Double-check all connections before powering on
- **Relay ratings**: Ensure relay can handle your device's current/voltage
- **Experience required**: Only proceed with high-voltage devices if you have experience
- **Isolation**: Relays provide isolation, but always be cautious with high voltage
- **Never work with live mains voltage** without proper training and safety measures

## Next Steps

Once you've mastered this example, try:

- **Token detection**: Trigger relay based on specific token receipts
- **NFT detection**: Activate relay when receiving specific NFTs
- **Multiple addresses**: Monitor multiple wallet addresses
- **Threshold triggers**: Only activate above certain balance amounts
- **Timed sequences**: Create complex relay activation patterns
- **Web interface**: Control relay remotely via web server
- **Multiple relays**: Control multiple devices independently

## Workshop Documentation

This example is part of a workshop. Learn more at:

[View Workshop](https://cardanothings.io/workshops/02-read-and-output/light-up-the-tree)

## Additional Resources

- [Koios API Documentation](https://preprod.koios.rest/) - Full API reference
- [ArduinoJSON Documentation](https://arduinojson.org/) - JSON parsing guide
- [ESP32 HTTPClient Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient) - HTTP client docs
- [Arduino digitalWrite() Reference](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/) - Learn about controlling digital pins
- [Relays Explained](https://www.youtube.com/watch?v=jXcdH1PgmMI) - Video tutorial explaining how relays work
- [PingPong Wallet](https://pingpongwallet.com/) - Send test transactions easily

## Security Notes

⚠️ **Important Security Considerations:**

- **Never commit WiFi credentials to version control** (Git)
- **Never commit wallet addresses** to version control (they're public but best practice)
- **Use environment variables or separate config files** for production code
- **HTTPS is automatic**: ESP32 HTTPClient handles SSL/TLS encryption
- **Wallet addresses are public**: They don't need to be secret, but be mindful of privacy
