# Cardano POS (Point of Sale)

A complete Cardano point-of-sale system running on ESP32 with TFT display, web interface, and on-chain transaction monitoring.

## Overview

This project implements a Cardano payment terminal that:
- Displays payment QR codes on a TFT screen
- Provides a web interface for creating payment requests
- Monitors on-chain transactions using the Koios API
- Stores transaction history in LittleFS
- Automatically updates transaction hashes when payments are confirmed

## Features

### Web Interface
- Create payment requests with ADA amount input
- View transaction history in a table
- Real-time transaction list updates (polls every 30 seconds)
- Responsive design with modern UI

### TFT Display
- Displays QR codes for Cardano payment URLs
- Shows transaction ID and ADA amount
- Displays "PLEASE PAY NOW!" message during payment waiting
- Shows success message when payment is received
- Automatically returns to blank screen after 10 seconds

### Transaction Management
- REST API for creating and retrieving transactions
- Automatic transaction ID generation
- Transaction amount includes ID for unique identification
- On-chain transaction monitoring via Koios API
- Automatic transaction hash updates when payment confirmed

## Hardware Requirements

- ESP32 development board
- TFT display compatible with TFT_eSPI library
- WiFi connectivity

## Software Dependencies

- **Arduino Libraries:**
  - `TFT_eSPI` - TFT display control
  - `qrcode_espi` - QR code generation
  - `ArduinoJson` - JSON parsing and serialization
  - `LittleFS` - File system for ESP32
  - `WebServer` - HTTP server (ESP32 built-in)
  - `HTTPClient` - HTTP client for API calls (ESP32 built-in)
  - `WiFi` - WiFi connectivity (ESP32 built-in)

## Setup Instructions

### 1. Configure Secrets

Copy `secrets.h.example` to `secrets.h` and fill in your credentials:

```cpp
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASSWORD "YourWiFiPassword"
#define PAYMENT_ADDRESS "addr_test1..."
#define KOIOS_API_URL "https://preprod.koios.rest/api/v1/address_utxos"
```

**Note:** For mainnet, change `KOIOS_API_URL` to `https://api.koios.rest/api/v1/address_utxos`

### 2. Upload Files to LittleFS

1. Place all files from the `data/` directory into your project's `data/` folder
2. In Arduino IDE: **Tools → ESP32 Sketch Data Upload**
3. Wait for upload to complete

**Required files:**
- `index.html` - Main web interface
- `styles.css` - Styling
- `requestPayment.js` - Payment request creation
- `transactionList.js` - Transaction list display
- `app.js` - Additional JavaScript (if needed)

### 3. Upload Sketch

1. Open `cardano-pos.ino` in Arduino IDE
2. Select your ESP32 board and port
3. Upload the sketch

### 4. Access Web Interface

1. Open Serial Monitor (115200 baud) to see the IP address
2. Navigate to `http://[ESP32_IP_ADDRESS]` in your browser
3. Create payment requests and view transactions

## Project Structure

```
cardano-pos/
├── cardano-pos.ino          # Main Arduino sketch
├── secrets.h                 # WiFi credentials and configuration (not in git)
├── secrets.h.example         # Template for secrets.h
├── wifi_manager.h/cpp        # WiFi connection management
├── web_server.h/cpp          # HTTP server and API endpoints
├── transaction_qr.h/cpp      # QR code display and transaction monitoring
├── data/                     # Web interface files (uploaded to LittleFS)
│   ├── index.html
│   ├── styles.css
│   ├── requestPayment.js
│   ├── transactionList.js
│   └── app.js
└── README.md                 # This file
```

## How It Works

### Payment Flow

1. **Create Payment Request:**
   - User enters ADA amount in web interface
   - Frontend converts ADA to lovelace and sends POST request
   - Backend creates transaction with auto-incremented ID
   - Transaction ID is added to amount: `storedAmount = amount + id`
   - Callback triggers TFT display to show QR code

2. **Display QR Code:**
   - TFT screen shows "PLEASE PAY NOW!" message
   - QR code displays Cardano payment URL: `web+cardano:[address]?amount=[ADA]`
   - QR amount includes the transaction ID
   - Transaction ID and ADA amount displayed below QR code

3. **Monitor Payment:**
   - System polls Koios API every 10 seconds
   - Checks for UTXO matching the payment address and amount
   - When payment found, updates transaction hash in JSON file
   - Displays success message for 10 seconds
   - Returns to blank screen

### Transaction Storage

Transactions are stored in `/transactions.json` in LittleFS:

```json
[
  {
    "id": 1,
    "amount": 5000001,
    "timestamp": 1234567890123,
    "txHash": "abc123..."
  }
]
```

- `id`: Auto-incremented transaction ID
- `amount`: Amount in lovelace (original amount + transaction ID)
- `timestamp`: Unix timestamp in milliseconds
- `txHash`: Transaction hash (empty until payment confirmed)

## API Endpoints

### GET `/api/transactions`
Returns all transactions as JSON array.

### POST `/api/transactions`
Creates a new transaction.

**Request:**
```json
{
  "amount": 5000000,
  "timestamp": 1234567890123
}
```

**Response:**
```json
{
  "id": 1,
  "amount": 5000001,
  "timestamp": 1234567890123,
  "txHash": ""
}
```

See `web_server.md` for detailed API documentation.

## Configuration

### Koios API

The system uses Koios API to check for on-chain transactions. Configure the API URL in `secrets.h`:

- **Preprod:** `https://preprod.koios.rest/api/v1/address_utxos`
- **Mainnet:** `https://api.koios.rest/api/v1/address_utxos`

### Payment Address

Set your Cardano payment address in `secrets.h`:

```cpp
#define PAYMENT_ADDRESS "addr_test1..."
```

## Module Documentation

- **WiFi Manager:** See `wifi_manager.md` for WiFi connection management
- **Web Server:** See `web_server.md` for HTTP server and API documentation
- **Transaction QR:** See `transaction_qr.md` for QR code display and transaction monitoring
- **Data Files:** See `data/README.md` for web interface file structure

## Troubleshooting

### WiFi Connection Issues
- Check `secrets.h` has correct WiFi credentials
- Ensure WiFi network is in range
- Check Serial Monitor for connection status

### Web Interface Not Loading
- Verify files were uploaded to LittleFS
- Check Serial Monitor for file listing
- Ensure web server started successfully

### QR Code Not Displaying
- Check TFT display connections
- Verify display initialization in `setup()`
- Check Serial Monitor for error messages

### Transactions Not Updating
- Verify Koios API URL is correct in `secrets.h`
- Check WiFi connection is active
- Review Serial Monitor for API request logs

## License

This project is part of the CardanoThings workshop series.
