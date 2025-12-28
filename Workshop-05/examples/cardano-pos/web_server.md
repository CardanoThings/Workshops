# Web Server

The web server module provides an HTTP server that serves static files from LittleFS and handles REST API endpoints for transaction management on the ESP32.

## Overview

This module handles:
- HTTP server initialization on port 80
- Serving static files from LittleFS filesystem
- REST API endpoints for transaction management
- Request routing and file handling
- Content type detection for various file types
- Fallback to index.html for missing files
- Transaction callback notifications for TFT display integration

## Functions

### `webServerSetup()`

Initializes the web server. **Must be called after WiFi is connected.**

**What it does:**
1. Initializes LittleFS filesystem
2. Lists all available files (for debugging)
3. Registers API endpoints (`/api/transactions`)
4. Sets up request handler for file serving
5. Starts the HTTP server on port 80

**Usage:**
```cpp
if (wifiManagerIsConnected()) {
  webServerSetup();
}
```

### `webServerLoop()`

Processes incoming HTTP requests. **Must be called regularly** in your `loop()` function.

**Usage:**
```cpp
void loop() {
  webServerLoop(); // Call regularly
  // ... other code
}
```

### `webServerIsRunning()`

Checks if the web server is currently running.

**Returns:**
- `true` if server is running
- `false` if server is not running

**Usage:**
```cpp
if (webServerIsRunning()) {
  Serial.println("Server is active");
}
```

### `setTransactionCreatedCallback(callback, display)`

Registers a callback function that will be called when a new transaction is created via the POST API endpoint. This allows the TFT display to immediately show the QR code for the new payment request.

**Parameters:**
- `callback`: Function pointer of type `TransactionCallback` that takes `(TFT_eSPI* display, int transactionId, uint64_t lovelaceAmount)`
- `display`: Pointer to the TFT display object

**Usage:**
```cpp
setTransactionCreatedCallback(displayNewTransactionQR, &display);
```

## API Endpoints

### GET `/api/transactions`

Retrieves all transactions from the `transactions.json` file stored in LittleFS.

**Response:**
- **200 OK**: Returns JSON array of transactions
- **500 Internal Server Error**: If file cannot be read

**Response Format:**
```json
[
  {
    "id": 1,
    "amount": 5000001,
    "timestamp": 1234567890123,
    "txHash": "abc123..."
  },
  {
    "id": 2,
    "amount": 10000002,
    "timestamp": 1234567890456,
    "txHash": ""
  }
]
```

**Notes:**
- Returns empty array `[]` if transactions file doesn't exist
- `amount` is in lovelace (with transaction ID added)
- `txHash` is empty string until payment is confirmed on-chain

### POST `/api/transactions`

Creates a new transaction and adds it to the `transactions.json` file.

**Request Body:**
```json
{
  "amount": 5000000,
  "timestamp": 1234567890123
}
```

**Request Fields:**
- `amount` (uint64_t, required): Amount in lovelace (1 ADA = 1,000,000 lovelace)
- `timestamp` (uint64_t, required): Unix timestamp in milliseconds

**Response:**
- **201 Created**: Returns the newly created transaction object
- **400 Bad Request**: If request body is missing or invalid
- **500 Internal Server Error**: If file cannot be written

**Response Format:**
```json
{
  "id": 3,
  "amount": 5000003,
  "timestamp": 1234567890123,
  "txHash": ""
}
```

**Notes:**
- Transaction ID is auto-incremented (finds highest existing ID and adds 1)
- The transaction ID is added to the amount: `storedAmount = amount + id`
- `txHash` is initially empty and will be populated when payment is confirmed
- Triggers the transaction callback if registered, allowing immediate QR code display

## How It Works

### File Serving

When a client requests a file:

1. **Path Normalization**: The requested path is normalized (ensures it starts with `/`)
2. **Root Path Handling**: Requests to `/` or empty path are redirected to `/index.html`
3. **File Existence Check**: Checks if the requested file exists in LittleFS
4. **Content Type Detection**: Determines MIME type based on file extension
5. **File Streaming**: If found, streams the file to the client with appropriate content type
6. **Fallback**: If file not found, tries to serve `index.html` as fallback
7. **404 Error**: If no file is found and no fallback available, returns 404 error

### Request Handling

The server uses a two-tier routing system:
- **API Routes**: Explicitly registered routes for `/api/transactions` (GET and POST)
- **File Routes**: `onNotFound()` handler catches all other requests and serves files from LittleFS

### Content Types

The server automatically detects and sets appropriate content types:
- HTML files: `text/html`
- CSS files: `text/css`
- JavaScript files: `application/javascript`
- JSON files: `application/json`
- Other files: `text/plain`

### Transaction Management

Transactions are stored in `/transactions.json` in LittleFS:
- File is created automatically on first POST request
- Transactions are stored as a JSON array
- Each transaction includes: `id`, `amount`, `timestamp`, `txHash`
- Transaction IDs are auto-incremented
- Transaction amounts include the ID (for unique payment identification)

## File Structure

Files in the `data/` directory are accessible at the root path:
- `data/index.html` → `http://[IP]/index.html` or `http://[IP]/`
- `data/styles.css` → `http://[IP]/styles.css`
- `data/app.js` → `http://[IP]/app.js`
- `data/requestPayment.js` → `http://[IP]/requestPayment.js`
- `data/transactionList.js` → `http://[IP]/transactionList.js`

## Key Features

- **Non-blocking**: Uses asynchronous request handling
- **REST API**: Provides GET and POST endpoints for transaction management
- **File-based Routing**: Simple routing based on file paths
- **Fallback Support**: Automatically serves `index.html` for missing files
- **LittleFS Integration**: Seamlessly works with LittleFS filesystem
- **Content Type Detection**: Automatically sets correct MIME types
- **Transaction Callbacks**: Integrates with TFT display for immediate QR code display
- **Error Handling**: Gracefully handles missing files and errors

## Requirements

- WiFi must be connected before calling `webServerSetup()`
- Files must be uploaded to LittleFS before they can be served
- Server must be initialized before calling `webServerLoop()`
- ArduinoJson library for JSON parsing and serialization

## Dependencies

- `WebServer.h` - ESP32 web server library
- `LittleFS.h` - LittleFS filesystem library
- `ArduinoJson.h` - JSON parsing and serialization
- `WiFi.h` - WiFi connectivity

## Limitations

- No authentication or security features
- Single-threaded request handling
- Transaction file size limited by available flash memory
- No concurrent transaction creation protection (single device use recommended)
