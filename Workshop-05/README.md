# Workshop 05: QR-Code Payments

In this workshop you will build a simple payment system right on your microcontroller utilising everything you have learned in former workshops. You will also learn about Cardano mobile wallets like Yoroi, Vespr or BeginWallet.

## Workshop Documentation

[View full workshop on cardanothings.io](https://cardanothings.io/workshops/05-qr-code-payments)

## Steps

1. **Building the Interface** - Build a simple payment user interface utilizing touch capabilities and TFT_eSPI Library
2. **CIP13 Integration** - Learn about Cardano Improvement Proposals (CIPs) and CIP13 for creating payment URIs
3. **QR-Code Creation** - Create and display a QR-Code with a CIP-13 Payment URI on your microcontroller display
4. **Confirming the payment** - Implement a listener for transactions and display confirmation

## Examples

This folder contains the following code examples:

- **`basic-webserver`** - A basic HTTP web server that serves static HTML files from LittleFS. Demonstrates WiFi connection management, LittleFS file system usage, and serving web content from the ESP32. Perfect starting point for understanding web server basics.

- **`qr-code-basics`** - Simple QR code generation and display example for TFT displays using sprites. Shows how to generate QR codes, render them to off-screen buffers, and display them on the screen. Essential foundation for creating payment interfaces with scannable QR codes.

- **`cardano-pos`** - Complete Cardano Point of Sale (POS) system with web interface, QR code payment display, and on-chain transaction monitoring. Features include:
  - Web-based payment request creation
  - QR code display on TFT screen with CIP-13 payment URIs
  - Transaction history management
  - Automatic transaction monitoring via Koios API
  - Real-time payment confirmation

## Requirements

**Hardware:**

- ESP32 development board (or compatible ESP32-based board like ESP32-C3)
- TFT display compatible with TFT_eSPI library (e.g., Cheap Yellow Display / CYD)
- Micro USB Cable for programming and power
- Computer with Arduino IDE installed
- WiFi Network with Internet Access

**Software:**

- Arduino IDE with ESP32 board support installed
- Required Arduino Libraries (install via Library Manager):
  - `TFT_eSPI` - TFT display control
  - `QRcodeDisplay` - QR code generation
  - `QRcode_eSPI` - TFT_eSPI adapter for QR codes
  - `ArduinoJson` - JSON parsing and serialization
  - `LittleFS` - File system for ESP32 (built-in)
  - `WebServer` - HTTP server (ESP32 built-in)
  - `HTTPClient` - HTTP client for API calls (ESP32 built-in)
- Cardano wallet (Yoroi, Vespr, or BeginWallet) for testing payments
- Node.js and NPM (for advanced examples, if needed)
