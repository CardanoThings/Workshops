# Transaction QR Display

The transaction QR module handles QR code generation, display on TFT screen, and on-chain transaction monitoring for the Cardano POS system.

## Overview

This module provides:
- QR code generation for Cardano payment URLs
- TFT display management for payment requests
- On-chain transaction monitoring via Koios API
- Automatic transaction hash updates
- Success message display with automatic timeout
- Precise ADA amount formatting (avoiding floating-point errors)

## Functions

### `transactionQRInit(display)`

Initializes the transaction QR display system. **Must be called once in `setup()` before using other functions.**

**Parameters:**
- `display`: Reference to the TFT_eSPI display object

**What it does:**
1. Calculates optimal QR code size based on display dimensions
2. Creates sprite buffer for QR code rendering
3. Initializes QR code generator
4. Resets all state variables

**Usage:**
```cpp
void setup() {
  // ... other setup code ...
  transactionQRInit(display);
}
```

### `displayTransactionQR(display, transactionId, lovelaceAmount)`

Displays a QR code for a transaction on the TFT screen. This is a standalone function that can be called to display any transaction's QR code.

**Parameters:**
- `display`: Reference to the TFT_eSPI display object
- `transactionId`: The transaction ID
- `lovelaceAmount`: Amount in lovelace (with transaction ID already added)

**What it does:**
1. Formats the Cardano payment URL with amount including ID
2. Generates QR code on sprite buffer
3. Displays QR code centered on screen
4. Shows transaction ID and ADA amount below QR code

**Usage:**
```cpp
displayTransactionQR(display, 1, 5000001);
```

### `displayNewTransactionQR(display, transactionId, lovelaceAmount)`

Displays QR code for a newly created transaction and starts monitoring for payment. This is typically called via callback when a new transaction is created through the API.

**Parameters:**
- `display`: Pointer to the TFT_eSPI display object
- `transactionId`: The transaction ID
- `lovelaceAmount`: Amount in lovelace (with transaction ID already added)

**What it does:**
1. Sets up waiting state for payment monitoring
2. Displays "PLEASE PAY NOW!" message
3. Generates and displays QR code
4. Shows transaction ID and ADA amount
5. Starts periodic checking for on-chain payment
6. Logs transaction details to Serial

**Usage:**
```cpp
// Typically called via callback from web server
displayNewTransactionQR(&display, 1, 5000001);
```

### `transactionQRUpdate(display)`

Update function that must be called regularly in `loop()`. Handles payment monitoring and success message timeout.

**Parameters:**
- `display`: Reference to the TFT_eSPI display object

**What it does:**
1. Checks if success message should be cleared (after 10 seconds)
2. If waiting for payment, checks Koios API every 10 seconds
3. Updates transaction hash when payment is found
4. Displays success message when payment confirmed
5. Automatically returns to blank screen after success timeout

**Usage:**
```cpp
void loop() {
  // ... other code ...
  transactionQRUpdate(display);
}
```

### `checkForTransaction(transactionId, lovelaceAmount)`

Checks the Koios API for a transaction matching the given amount and payment address.

**Parameters:**
- `transactionId`: The transaction ID to check
- `lovelaceAmount`: Amount in lovelace (with ID already added)

**Returns:**
- `String`: Transaction hash if payment found, empty string otherwise

**What it does:**
1. Builds Koios API URL with amount filter
2. Sends POST request with payment address
3. Parses response for matching UTXO
4. Returns transaction hash if found
5. Logs all steps to Serial for debugging

**Internal function** - called automatically by `transactionQRUpdate()`.

### `displaySuccessAndUpdateHash(display, transactionId, txHash)`

Displays success message and updates the transaction hash in the JSON file.

**Parameters:**
- `display`: Reference to the TFT_eSPI display object
- `transactionId`: The transaction ID
- `txHash`: The transaction hash from the blockchain

**What it does:**
1. Updates transaction hash in `/transactions.json`
2. Displays "Payment Received!" message on screen
3. Starts 10-second timer for success message
4. Logs success to Serial

**Internal function** - called automatically when payment is detected.

### `updateTransactionHash(transactionId, txHash)`

Helper function to update a transaction's hash in the LittleFS JSON file.

**Parameters:**
- `transactionId`: The transaction ID to update
- `txHash`: The transaction hash to store

**Returns:**
- `bool`: `true` if update successful, `false` otherwise

**Internal function** - used by `displaySuccessAndUpdateHash()`.

### `formatLovelaceToADA(lovelaceAmount)`

Helper function to format lovelace amounts to ADA strings with precise formatting, avoiding floating-point precision errors.

**Parameters:**
- `lovelaceAmount`: Amount in lovelace (uint64_t)

**Returns:**
- `String`: Formatted ADA amount (e.g., "12.000003")

**What it does:**
1. Uses integer division to get whole ADA part
2. Uses modulo to get fractional lovelace part
3. Formats with exactly 6 decimal places
4. Adds leading zeros for fractional part

**Example:**
- Input: `12000003` lovelace
- Output: `"12.000003"` ADA

**Internal function** - used for QR code URL generation.

## How It Works

### Payment Flow

1. **Transaction Created:**
   - New transaction created via POST `/api/transactions`
   - Web server callback triggers `displayNewTransactionQR()`
   - QR code displayed with "PLEASE PAY NOW!" message

2. **QR Code Display:**
   - QR code contains: `web+cardano:[PAYMENT_ADDRESS]?amount=[ADA]`
   - Amount includes transaction ID for unique identification
   - Transaction ID and ADA amount shown below QR code

3. **Payment Monitoring:**
   - `transactionQRUpdate()` called regularly in `loop()`
   - Every 10 seconds, checks Koios API for matching UTXO
   - Checks for exact amount (including ID) at payment address

4. **Payment Confirmed:**
   - When UTXO found, transaction hash extracted
   - Transaction hash updated in `/transactions.json`
   - Success message displayed for 10 seconds
   - Screen returns to blank after timeout

### QR Code Format

The QR code contains a Cardano payment URL in the format:
```
web+cardano:[PAYMENT_ADDRESS]?amount=[ADA_AMOUNT]
```

**Example:**
```
web+cardano:addr_test1...?amount=12.000003
```

**Important:** The amount in the QR code includes the transaction ID. For example:
- Original amount: 12 ADA (12,000,000 lovelace)
- Transaction ID: 3
- QR amount: 12.000003 ADA (12,000,003 lovelace)

This ensures each payment request has a unique amount, making it easy to identify which transaction was paid.

### On-Chain Monitoring

The system uses the Koios API to check for incoming payments:

1. **API Endpoint:** Configured in `secrets.h` as `KOIOS_API_URL`
2. **Request Format:** POST request with address filter and amount filter
3. **Response:** Array of UTXOs matching the criteria
4. **Matching:** Checks for exact amount (including transaction ID)

**API Request Example:**
```
POST https://preprod.koios.rest/api/v1/address_utxos?value=eq.12000003
Body: {"_addresses": ["addr_test1..."]}
```

**Response Example:**
```json
[
  {
    "tx_hash": "abc123...",
    "value": "12000003",
    ...
  }
]
```

### Display States

The module manages several display states:

1. **Waiting for Payment:**
   - White background
   - "PLEASE PAY NOW!" message (size 2, centered)
   - QR code centered below message
   - Transaction ID (left-aligned, 25px padding from QR left edge)
   - ADA amount (right-aligned, 25px padding from QR right edge)

2. **Payment Received:**
   - Black background
   - "Payment Received!" message (size 2, centered)
   - Displayed for 10 seconds
   - Automatically clears to blank screen

3. **Blank Screen:**
   - Black background
   - No content displayed
   - Ready for next transaction

## Configuration

### Required Configuration in `secrets.h`

```cpp
// Payment address where payments should be sent
#define PAYMENT_ADDRESS "addr_test1..."

// Koios API URL for checking transactions
// Preprod: https://preprod.koios.rest/api/v1/address_utxos
// Mainnet: https://api.koios.rest/api/v1/address_utxos
#define KOIOS_API_URL "https://preprod.koios.rest/api/v1/address_utxos"
```

### Timing Constants

- **CHECK_INTERVAL:** 10 seconds (10000ms) - How often to check for payment
- **SUCCESS_DISPLAY_TIME:** 10 seconds (10000ms) - How long to show success message

These can be modified in the `namespace` section of `transaction_qr.cpp` if needed.

## Dependencies

- **TFT_eSPI:** TFT display control library
- **qrcode_espi:** QR code generation library
- **ArduinoJson:** JSON parsing and serialization
- **HTTPClient:** HTTP client for API requests (ESP32 built-in)
- **LittleFS:** File system for reading/writing transaction JSON
- **WiFi:** WiFi connectivity (ESP32 built-in)
- **secrets.h:** Configuration file with `PAYMENT_ADDRESS` and `KOIOS_API_URL`

## Integration

### With Web Server

The module integrates with the web server via callback:

```cpp
// In setup()
setTransactionCreatedCallback(displayNewTransactionQR, &display);
```

When a new transaction is created via POST `/api/transactions`, the web server automatically calls this callback, triggering the QR code display.

### With Main Loop

The module must be updated regularly:

```cpp
void loop() {
  // ... other code ...
  transactionQRUpdate(display); // Must be called regularly
}
```

## Key Features

- **Precise Amount Formatting:** Uses integer arithmetic to avoid floating-point errors
- **Automatic Monitoring:** Periodically checks for payments without blocking
- **State Management:** Handles multiple display states (waiting, success, blank)
- **Transaction Updates:** Automatically updates transaction hashes in JSON file
- **Serial Logging:** Comprehensive logging for debugging
- **Non-blocking:** All operations are non-blocking, allowing other code to run
- **Error Handling:** Gracefully handles API errors and network issues

## Error Handling

The module handles various error scenarios:

- **WiFi Disconnected:** Skips API check, logs message, continues waiting
- **API Request Failed:** Logs error, continues checking on next interval
- **JSON Parse Error:** Logs error, continues monitoring
- **File Write Error:** Logs error, but still displays success message
- **No Payment Found:** Continues checking on next interval

All errors are logged to Serial but don't crash the system.

## Serial Logging

The module provides detailed Serial logging for debugging:

- Transaction listener start/stop
- Payment check attempts
- API request/response details
- Payment confirmation
- Success message timeout
- Error messages

**Example Serial Output:**
```
========================================
[Transaction Listener] Starting to listen for payment
  Transaction ID: 1
  Amount: 12.000000 ADA (12000000 lovelace)
  Payment Address: addr_test1...
  Check interval: 10 seconds
========================================
[Transaction Listener] Checking payment (waiting for 10 seconds)...
[Transaction Check] Checking for payment - TX ID: 112000003 lovelace)
[Transaction Check] Sending request to: https://preprod.koios.rest/api/v1/address_utxos?value=eq.12000003
[Transaction Check] Payment found! Transaction hash: abc123...
[Transaction Listener] Payment confirmed! Stopping listener.
Payment received! Transaction hash: abc123...
Success message will be shown for 10 seconds
Success message cleared, returning to blank screen
```

## Limitations

- **Single Transaction:** Only monitors one transaction at a time
- **Network Dependent:** Requires WiFi connection for API checks
- **API Rate Limits:** Subject to Koios API rate limits (10-second intervals help)
- **Display Size:** QR code size limited by display dimensions
- **Transaction File:** Requires `/transactions.json` to exist for hash updates

## Troubleshooting

### QR Code Not Displaying
- Check TFT display initialization in `setup()`
- Verify `transactionQRInit()` was called
- Check Serial Monitor for error messages
- Ensure display connections are correct

### Payment Not Detected
- Verify WiFi is connected
- Check Koios API URL in `secrets.h`
- Review Serial Monitor for API request/response
- Ensure payment amount matches exactly (including ID)
- Check payment address is correct

### Success Message Not Clearing
- Verify `transactionQRUpdate()` is called in `loop()`
- Check Serial Monitor for timeout messages
- Ensure no blocking code preventing loop execution

### Transaction Hash Not Updating
- Check `/transactions.json` exists in LittleFS
- Verify file permissions
- Review Serial Monitor for file write errors
- Ensure transaction ID matches existing transaction

