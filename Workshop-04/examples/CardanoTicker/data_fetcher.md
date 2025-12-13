# WiFi Manager & Data Fetcher

These two modules handle the core functionality of connecting to the internet and fetching blockchain data. They work together to keep your ticker connected and up-to-date.

## WiFi Manager

The WiFi manager handles connecting to WiFi and automatically reconnecting if the connection drops. It's a simple wrapper around the ESP32 WiFi library that provides:

- **Automatic reconnection**: If WiFi disconnects, it automatically tries to reconnect
- **Connection status checking**: Provides a simple function to check if WiFi is connected
- **Non-blocking design**: Doesn't freeze the program while connecting

### Key Functions

- `wifiManagerSetup(ssid, password)`: Initialize WiFi with your credentials (call once in setup)
- `wifiManagerLoop()`: Check connection status and reconnect if needed (call in loop)
- `wifiManagerIsConnected()`: Returns true if WiFi is connected, false otherwise

### How It Works

The WiFi manager uses the same WiFi connection techniques you learned in Workshop 02, but organized into a reusable module. It checks the connection status periodically and automatically attempts to reconnect if disconnected.

## Data Fetcher

The data fetcher organizes all the API calls (Koios, MinSwap, and Cexplorer) into a reusable module. It fetches data periodically and stores it for the screens to display.

### Key Features

- **Rate limiting**: Prevents excessive API calls
  - Wallet balance: Updates every 1 minute (Koios API)
  - Tokens/NFTs: Updates every 10 minutes (MinSwap/Cexplorer APIs)
- **Data storage**: Stores fetched data in arrays for easy access
- **Getter functions**: Provides simple functions like `getWalletBalance()` and `getToken(i)` for screens to use

### Key Functions

**Initialization:**
- `initDataFetcher()`: Initialize data storage (call once in setup)

**Update Functions (call in loop):**
- `updateKoiosData()`: Fetches wallet balance from Koios API (every 1 minute)
- `updatePortfolioData()`: Fetches tokens and NFTs from MinSwap/Cexplorer (every 10 minutes)

**Getter Functions (for screens to use):**
- `getWalletBalance()`: Returns your ADA balance
- `getTokenCount()`: Returns number of tokens you own
- `getNftCount()`: Returns number of NFT collections you own
- `getToken(i)`: Returns token data at index i
- `getNFT(i)`: Returns NFT collection data at index i
- `getLastKoiosFetchTime()`: Returns timestamp of last wallet balance fetch

### How It Works

The data fetcher uses rate limiting to avoid overwhelming APIs. Each update function checks if enough time has passed since the last fetch before making a new API call. This ensures:

1. **Efficient API usage**: Doesn't make unnecessary requests
2. **Respects API limits**: Stays within reasonable rate limits
3. **Smooth operation**: Data updates happen automatically in the background

### Data Structures

**TokenInfo**: Stores information about a token
- `ticker`: Token symbol (e.g., "MIN", "ADA")
- `amount`: How many tokens you own
- `value`: Total value in USD
- `change24h`: 24-hour price change percentage

**NFTInfo**: Stores information about an NFT collection
- `name`: Collection name (e.g., "Cardano Punks")
- `amount`: Number of NFTs you own from this collection
- `floorPrice`: Lowest selling price in ADA
- `policyId`: Unique identifier for the collection

### API Integration

The data fetcher integrates with three Cardano APIs:

1. **Koios API**: Fetches wallet balance from your stake address
2. **MinSwap API**: Fetches token positions and NFT collections from your wallet address
3. **Cexplorer API**: Fetches NFT floor prices using Policy IDs from MinSwap

All three APIs use the same HTTP request and JSON parsing techniques you learned in Workshop 02, just organized into a reusable module!





