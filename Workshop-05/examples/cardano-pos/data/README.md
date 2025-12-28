# Data Directory

This directory contains all the static files that will be served by the ESP32 web server. Files placed in this directory are uploaded to LittleFS and made accessible via HTTP.

## Overview

The `data` directory contains the complete web interface for the Cardano POS system:
- HTML files for the user interface
- CSS stylesheets for styling
- JavaScript files for functionality
- Static assets (favicon, etc.)

## File Serving

Files in this directory are served at the root path of your web server. For example:

- `data/index.html` → accessible at `http://[ESP32_IP]/index.html` or `http://[ESP32_IP]/`
- `data/styles.css` → accessible at `http://[ESP32_IP]/styles.css`
- `data/requestPayment.js` → accessible at `http://[ESP32_IP]/requestPayment.js`
- `data/transactionList.js` → accessible at `http://[ESP32_IP]/transactionList.js`

## Uploading Files

To upload files from this directory to your ESP32:

1. **Arduino IDE**: Use **Tools → ESP32 Sketch Data Upload** to upload all files in this directory to LittleFS
2. **PlatformIO**: Use the filesystem upload feature in PlatformIO

**Important**: Files must be uploaded to LittleFS before they can be served by the web server. Simply placing files in this directory is not enough - they need to be uploaded to the ESP32's flash memory.

## Current Files

### `index.html`

The main HTML page served at the root path. Contains:

- Payment request creation button
- Transaction list display area
- Modal dialog for payment amount input

### `styles.css`

Stylesheet for the web interface. Includes:

- Button styling
- Modal dialog styling (using native `<dialog>` element)
- Transaction table styling
- Responsive design

### `requestPayment.js`

Handles payment request creation:

- Opens modal dialog for ADA amount input
- Converts ADA to lovelace (1 ADA = 1,000,000 lovelace)
- Sends POST request to `/api/transactions`
- Refreshes transaction list after successful creation
- Uses native `<dialog>` element for modal

### `transactionList.js`

Manages transaction list display:

- Fetches transactions from `/api/transactions` API
- Displays transactions in a table format
- Converts lovelace amounts to ADA for display
- Formats timestamps to readable dates
- Auto-refreshes every 30 seconds
- Exposes `window.refreshTransactions()` for manual refresh

### `transactions.json`

Transaction storage file (created automatically by the API, not manually uploaded).

### `favicon.ico`

Favicon for the web interface.

## API Integration

The frontend JavaScript files interact with the ESP32 web server API:

### GET `/api/transactions`

Fetched by `transactionList.js` to retrieve all transactions.

**Response:**

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

### POST `/api/transactions`

Called by `requestPayment.js` to create new payment requests.

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

## Adding New Files

To add new files:

1. Place the file in this `data` directory (or a subdirectory)
2. Upload the files to LittleFS using Arduino IDE or PlatformIO
3. Access the file via `http://[ESP32_IP]/filename`

## File Structure

```
data/
├── index.html              # Main HTML page
├── styles.css              # Stylesheet
├── requestPayment.js       # Payment request creation
├── transactionList.js      # Transaction list management
├── app.js                  # Additional JavaScript
├── favicon.ico             # Favicon
└── transactions.json       # Transaction storage (auto-created)
```

All files maintain their directory structure when uploaded to LittleFS.

## Notes

- File names are case-sensitive
- The web server automatically serves `index.html` for root path requests (`/`)
- If a requested file doesn't exist, the server will try to serve `index.html` as a fallback
- File size is limited by available flash memory on your ESP32
- JavaScript files use modern ES6+ features (async/await, fetch API)
- The interface uses native HTML5 features (dialog element, no jQuery or frameworks)

## Frontend Features

### Payment Request Creation

- Modal dialog using native `<dialog>` element
- Input validation for ADA amounts
- Automatic conversion to lovelace
- Error handling with console logging
- Automatic transaction list refresh after creation

### Transaction List

- Real-time updates (polls every 30 seconds)
- Manual refresh capability
- Table display with formatted data
- Empty transaction hash display as "-"
- Timestamp formatting to local date/time
- ADA amount display (converted from lovelace)
