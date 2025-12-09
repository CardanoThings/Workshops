# Cardano Ticker

A real-time Cardano portfolio display for ESP32 microcontrollers with TFT displays. This project fetches your Cardano wallet balance, token holdings, and NFT collection data from blockchain APIs and displays them on a rotating set of screens with a scrolling price ticker.

## Overview

The Cardano Ticker is an embedded system that connects to WiFi and continuously fetches your Cardano blockchain data from multiple APIs:
- **Koios API**: Wallet balance (ADA)
- **MinSwap API**: Token positions and NFT collections
- **Cexplorer API**: NFT floor prices

The device displays this information on a TFT display through four rotating screens, with a scrolling ticker showing token prices at the bottom.

## Hardware Requirements

- **ESP32 microcontroller** (or compatible board)
- **TFT display** (e.g., CYD - Cheap Yellow Display)
- **WiFi connection** (for fetching blockchain data)
- **USB cable** (for programming and power)

## Software Dependencies

### Required Libraries

Install these libraries in the Arduino IDE Library Manager:

- **TFT_eSPI** - TFT display library for ESP32
- **SPI** - Serial Peripheral Interface (usually included with Arduino)

### Arduino IDE Setup

1. Install Arduino IDE (1.8.x or 2.x)
2. Add ESP32 board support:
   - Go to `File > Preferences`
   - Add this URL to "Additional Board Manager URLs": `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to `Tools > Board > Boards Manager`
   - Search for "ESP32" and install

## Project Structure

The project is organized into modular components:

```
CardanoTicker/
├── CardanoTicker.ino    # Main entry point (setup & loop)
├── config.h/cpp         # Configuration (wallet addresses, API URLs)
├── secrets.h            # WiFi credentials (create from secrets.h.example)
├── wifi_manager.h/cpp   # WiFi connection management
├── data_fetcher.h/cpp   # Blockchain API data fetching
├── datascreens.h        # Screen drawing function declarations
├── wallet_screen.h/cpp  # Wallet balance screen
├── token_screen.h/cpp   # Token holdings screen
├── nft_screen.h/cpp     # NFT collection screen
├── status_screen.h/cpp  # System status screen
├── ticker.h/cpp         # Scrolling price ticker
├── startscreen.h/cpp    # Startup splash screen
└── screen_helper.h/cpp  # Screen rendering utilities
```

## How It Works

### Architecture Overview

The project follows a modular architecture where each component handles a specific responsibility:

1. **Main Loop (`CardanoTicker.ino`)**: Orchestrates all components
2. **WiFi Manager**: Handles connection and reconnection
3. **Data Fetcher**: Fetches data from blockchain APIs with rate limiting
4. **Screen Modules**: Draw different views of the data
5. **Ticker**: Displays scrolling token prices

### Program Flow

#### Initialization (`setup()`)

When the device starts:

1. **Serial Communication**: Initializes debugging output (115200 baud)
2. **Display Setup**: 
   - Initializes TFT display
   - Sets rotation to landscape
   - Inverts colors (for CYD displays)
3. **Start Screen**: Shows "Cardano Ticker" splash screen
4. **WiFi Connection**: 
   - Attempts to connect to WiFi
   - Waits up to 30 seconds for connection
5. **Data Initialization**: 
   - Initializes data storage structures
   - Fetches initial data if WiFi connected
6. **Ticker Setup**: Initializes scrolling ticker
7. **First Screen**: Displays the wallet screen

#### Main Loop (`loop()`)

The main loop runs continuously and handles:

1. **WiFi Maintenance**: Keeps connection alive, handles reconnection
2. **Data Updates**: 
   - Wallet balance: Updates every 1 minute (Koios API)
   - Tokens/NFTs: Updates every 10 minutes (MinSwap/Cexplorer APIs)
   - Functions check internally if enough time has passed
3. **Screen Rotation**: 
   - Switches between 4 screens every 10 seconds
   - Cycle: Wallet → Tokens → NFTs → Status → Wallet...
4. **Ticker Animation**: Updates scrolling ticker for smooth animation

### Screen Rotation

The device cycles through 4 screens:

1. **Wallet Screen** (`currentScreenIndex = 0`): Shows ADA balance
2. **Token Screen** (`currentScreenIndex = 1`): Shows token holdings with prices
3. **NFT Screen** (`currentScreenIndex = 2`): Shows NFT collections with floor prices
4. **Status Screen** (`currentScreenIndex = 3`): Shows WiFi status, uptime, last update time

Each screen is displayed for 10 seconds before rotating to the next.

### Data Fetching

The data fetcher uses rate limiting to avoid overwhelming APIs:

- **Koios Data** (Wallet Balance):
  - Fetches every 1 minute
  - Uses your stake address
  - Returns ADA balance in Lovelace (converted to ADA)

- **Portfolio Data** (Tokens & NFTs):
  - Fetches every 10 minutes
  - Uses your wallet address
  - Fetches token positions from MinSwap
  - Fetches NFT collections from MinSwap
  - Fetches NFT floor prices from Cexplorer
  - Stores up to 8 tokens and 8 NFT collections

### Scrolling Ticker

The ticker at the bottom of the screen:
- Shows token prices scrolling horizontally
- Updates frequently for smooth animation
- Displays format: `TOKEN: $PRICE (+CHANGE%)`

## Setup Instructions

### 1. Clone/Download the Project

Download all files from the `CardanoTicker` directory.

### 2. Configure WiFi Credentials

1. Copy `secrets.h.example` to `secrets.h`
2. Edit `secrets.h` and add your WiFi credentials:
   ```cpp
   #define WIFI_SSID "YourWiFiNetworkName"
   #define WIFI_PASSWORD "YourWiFiPassword"
   #define CEXPLORER_API_KEY "YourAPIKey"  // Optional, for NFT floor prices
   ```

**Important**: `secrets.h` should NOT be committed to git (it's in `.gitignore`)

### 3. Configure Wallet Addresses

1. Open `config.cpp`
2. Edit the following variables:
   ```cpp
   String stakeAddress = "stake1...";  // Your Cardano stake address
   String walletAddress = "addr1..."; // Your Cardano wallet address
   ```

**Finding your addresses:**
- **Stake Address**: Found in wallet apps (Daedalus, Yoroi, Eternl). Starts with `stake1...` on mainnet
- **Wallet Address**: Your payment address. Starts with `addr1...` on mainnet

### 4. Configure TFT Display

If using a different TFT display, you may need to edit `TFT_eSPI` library settings:
- Open Arduino IDE
- Go to `File > Preferences`
- Find your Arduino sketchbook folder
- Navigate to `libraries/TFT_eSPI/User_Setup.h`
- Configure pins and display settings for your specific display

### 5. Upload to ESP32

1. Connect ESP32 to computer via USB
2. In Arduino IDE:
   - Select board: `Tools > Board > ESP32 Arduino > [Your ESP32 Board]`
   - Select port: `Tools > Port > [Your COM Port]`
   - Click "Upload" button
3. Open Serial Monitor (`Tools > Serial Monitor`) at 115200 baud to see debug messages

## Configuration Options

### Screen Duration

To change how long each screen is displayed, edit `CardanoTicker.ino`:

```cpp
const unsigned long SCREEN_DURATION_MS = 10000UL; // 10 seconds (change to your preference)
```

### Data Update Intervals

To change update frequencies, edit `data_fetcher.cpp`:

- Wallet balance update interval (default: 1 minute)
- Portfolio update interval (default: 10 minutes)

### API Endpoints

API URLs are defined in `config.cpp`. You can change them if needed:

```cpp
const char *koiosApiUrl = "https://api.koios.rest/api/v0/...";
const char *minswapApiUrl = "https://api.minswap.org/...";
const char *cexplorerApiUrl = "https://api.cexplorer.io/...";
```

## Troubleshooting

### WiFi Connection Issues

- Check WiFi credentials in `secrets.h`
- Ensure ESP32 is within range of WiFi router
- Check Serial Monitor for connection errors
- The device will keep trying to reconnect automatically

### Display Issues

- Verify TFT display wiring (SPI connections)
- Check if display needs color inversion (already set for CYD)
- Adjust rotation if display is upside down: `tft.setRotation(0-3)`

### No Data Displayed

- Check Serial Monitor for API errors
- Verify wallet addresses in `config.cpp` are correct
- Ensure wallet has balance/tokens/NFTs to display
- Check internet connection (WiFi must be connected)

### API Rate Limiting

If you see errors about rate limiting:
- Increase update intervals in `data_fetcher.cpp`
- The code already implements rate limiting, but APIs may have stricter limits

## Understanding the Code

### Key Concepts

**Arduino Programming Model:**
- `setup()`: Runs once at startup - initialization
- `loop()`: Runs forever - main program logic

**Non-blocking Design:**
- Uses `millis()` for timing instead of `delay()`
- Allows multiple tasks to run simultaneously
- WiFi, data fetching, and display updates all happen concurrently

**Modular Architecture:**
- Each `.h/.cpp` pair is a self-contained module
- Functions are organized by responsibility
- Makes code easier to understand and modify

**Rate Limiting:**
- Data fetchers check time since last update
- Prevents excessive API calls
- Saves bandwidth and respects API limits

## API Information

### Koios API
- **Purpose**: Wallet balance queries
- **Rate Limit**: Generally generous, but be respectful
- **Documentation**: https://api.koios.rest/

### MinSwap API
- **Purpose**: Token positions and NFT collections
- **Rate Limit**: Check MinSwap documentation
- **Documentation**: https://api.minswap.org/

### Cexplorer API
- **Purpose**: NFT floor prices
- **Rate Limit**: May require API key for higher limits
- **Documentation**: https://cexplorer.io/api

## Support

For issues or questions, please check:
- Serial Monitor output for error messages
- API documentation for endpoint changes
- ESP32 and TFT_eSPI library documentation

