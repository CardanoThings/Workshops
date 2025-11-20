# Workshop 02: Read and Output

This workshop will teach you how to read data from the blockchain in intervals and use this data to trigger an action on your Microcontroller. You will learn about two other Cardano APIs and how to set them up and get a short introduction into connecting external hardware to your microcontroller.

## Workshop Documentation

[View full workshop on cardanothings.io](https://cardanothings.io/workshops/02-read-and-output)

## Steps

1. **Fetch your Wallet Balance** - Fetch wallet balance in intervals, output data to console and listen for changes
2. **Display data on your Microcontroller** - Learn the Arduino TFT_eSPI Library and display fetched data on TFT-Display
3. **Light up the tree** - Use external hardware like a 110V/220V relay to turn a lightbulb on and off based on on-chain events
4. **Epoch Clock** - Build a physical Epoch Clock with a D1 Microcontroller and LED Matrix

## Examples

This folder contains the following code examples:

- `wallet-balance-code` - Fetch wallet balance from APIs
- `blockfrost-code` - Alternative API implementation using Blockfrost
- `tft-display-code` - Basic TFT display example
- `formatted-display-code` - Formatted text display on TFT
- `relay-code` - Relay control based on blockchain events
- `led-code` - LED control example
- `epoch-clock-code` - LED matrix epoch clock
- `simple-epoch-code` - Simplified epoch display

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

