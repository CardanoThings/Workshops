# CardanoPOS - Arduino Point of Sale System

A Cardano payment point-of-sale system built for ESP32 with TFT display, WiFi connectivity, and web interface.

## Features

- **TFT Display**: Shows QR codes and payment information
- **Web Server**: Serves HTML/CSS/JS files from LittleFS
- **Payment Requests**: Create and manage ADA payment transactions
- **WiFi Connectivity**: Automatic WiFi connection management

## Required Libraries

### 1. QRcodeDisplay
Base QR code library for generating QR codes.

- **GitHub**: https://github.com/yoprogramo/QRcodeDisplay
- **Installation**:
  - **Arduino Library Manager**: Sketch → Include Library → Manage Libraries → Search "QRcodeDisplay" → Install
  - **Manual**: Download ZIP from GitHub → Sketch → Include Library → Add .ZIP Library

### 2. QRcode_eSPI
TFT_eSPI adapter for QR codes - allows QR codes to be drawn on TFT displays.

- **GitHub**: https://github.com/yoprogramo/QRcode_eSPI
- **Installation**:
  - **Arduino Library Manager**: Sketch → Include Library → Manage Libraries → Search "QRcode_eSPI" → Install
  - **Manual**: Download ZIP from GitHub → Sketch → Include Library → Add .ZIP Library

### 3. TFT_eSPI
TFT display library for ESP32 (usually comes with ESP32 board package).

- **Installation**: Usually included with ESP32 board support package
- If not available: Sketch → Include Library → Manage Libraries → Search "TFT_eSPI" → Install

### 4. ArduinoJson
JSON parsing library for handling transaction data.

- **Installation**: Sketch → Include Library → Manage Libraries → Search "ArduinoJson" by Benoit Blanchon → Install

### 5. LittleFS Upload Tool
Plugin for uploading files to LittleFS filesystem.

- **GitHub**: https://github.com/earlephilhower/arduino-littlefs-upload
- **Installation**:
  1. Download the VSIX file from the [releases page](https://github.com/earlephilhower/arduino-littlefs-upload/releases)
  2. Copy to:
     - **Mac/Linux**: `~/.arduinoIDE/plugins/`
     - **Windows**: `C:\Users\<username>\.arduinoIDE\plugins\`
  3. Restart Arduino IDE
  4. **Usage**: See "Upload Files to LittleFS" section below for detailed instructions

## Setup Instructions

### 1. Hardware Setup
- ESP32 development board
- TFT display compatible with TFT_eSPI
- Configure TFT_eSPI `User_Setup.h` for your display

### 2. Software Configuration

1. **Create `secrets.h`**:
   ```cpp
   #ifndef SECRETS_H
   #define SECRETS_H
   
   #define WIFI_SSID "YourWiFiSSID"
   #define WIFI_PASSWORD "YourWiFiPassword"
   
   #endif
   ```

2. **Update Cardano Address**:
   - Open `web_server.cpp`
   - Replace `"addr1qy..."` with your actual Cardano address (line 12-13)

### 3. Upload Files to LittleFS

1. Place your HTML, CSS, and JS files in the `data/` directory:
   ```
   CardanoPOS/
   ├── CardanoPOS.ino
   ├── data/
   │   ├── index.html
   │   ├── style.css
   │   └── app.js
   ```

2. **Upload to LittleFS**:
   
   **On Windows/Linux:**
   - Press `[Ctrl] + [Shift] + [P]` to open the Command Palette
   - Type and select: **"Upload LittleFS to Pico/ESP8266/ESP32"**
   
   **On macOS:**
   - Press `[⌘] + [Shift] + [P]` to open the Command Palette
   - Type and select: **"Upload LittleFS to Pico/ESP8266/ESP32"**
   
   The files from the `data/` directory will be uploaded to the ESP32's LittleFS filesystem.

### 4. Upload Sketch

1. Select your ESP32 board in Tools → Board
2. Select the correct port in Tools → Port
3. Click Upload

## Usage

1. Power on the device
2. Wait for WiFi connection (check Serial Monitor for IP address)
3. Access the web interface at `http://<ESP32_IP_ADDRESS>`
4. Enter ADA amount and click "Request Payment"
5. The TFT display will show a QR code and payment details

## File Structure

```
CardanoPOS/
├── CardanoPOS.ino          # Main sketch file
├── secrets.h                # WiFi credentials (not in git)
├── startscreen.h/cpp        # Startup screen display
├── wifi_manager.h/cpp       # WiFi connection management
├── web_server.h/cpp         # HTTP web server
├── transaction_display.h/cpp # Transaction display with QR code
└── data/                    # Web files (uploaded to LittleFS)
    ├── index.html
    ├── style.css
    └── app.js
```

## API Endpoints

- `POST /api/transactions` - Create a new payment transaction
  - Body: `{"amount": <lovelace>}`
  - Returns: `{"success": true, "id": <transaction_id>}`

## Transaction Storage

Transactions are stored in `/transactions.json` in LittleFS with the following structure:
```json
[
  {
    "amount": <lovelace>,
    "address": "<cardano_address>",
    "timestamp": <unix_timestamp>,
    "txHash": ""
  }
]
```

## Troubleshooting

- **QR code not displaying**: Check that QRcodeDisplay and QRcode_eSPI libraries are installed
- **Web server not starting**: Verify WiFi connection and check Serial Monitor for errors
- **Files not loading**: Ensure files are uploaded to LittleFS using the upload tool
- **Compilation errors**: Verify all required libraries are installed

## License

MIT License

