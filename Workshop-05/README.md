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

- `payment-ui-code` - Complete payment UI with number pad
- `simple-ui-code` - Basic UI example
- `cip-13-uri-code` - CIP-13 payment URI creation
- `qr-code-code` - QR code generation and display
- `simple-qr-code` - Basic QR code example
- `payment-monitor-code` - Transaction monitoring and confirmation
- `simple-monitor-code` - Basic transaction monitoring

## Requirements

**Hardware:**
- D1 Mini Microcontroller (ESP8266 based) with TFT Display and Touchscreen
- Micro USB Cable
- Computer with Arduino IDE installed
- WiFi Network with Internet Access

**Software:**
- Yoroi Wallet (Preprod Testnet)
- Arduino IDE
- NodeJs and NPM

