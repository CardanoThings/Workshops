/**
 * data_fetcher.cpp - Implementation of blockchain data fetching
 *
 * This file contains all the code that talks to Cardano blockchain APIs to
 * fetch your wallet data. It handles:
 * - Fetching wallet balance from Koios API
 * - Fetching token positions from MinSwap API
 * - Fetching NFT collection data from MinSwap and Cexplorer APIs
 * - Storing and organizing all this data for display
 *
 * Key Concepts:
 * - HTTP requests: How we talk to APIs over the internet
 * - JSON parsing: APIs return data in JSON format, we need to extract it
 * - Rate limiting: We don't fetch too often to avoid hitting API limits
 */

#include "data_fetcher.h"

// Libraries for making HTTP requests and parsing JSON responses
#include <ArduinoJson.h> // Parses JSON data from APIs
#include <HTTPClient.h>  // Makes HTTP requests (GET, POST) to APIs
#include <WiFi.h>        // WiFi functionality

// Our custom headers
#include "config.h"       // API URLs and wallet addresses
#include "wifi_manager.h" // WiFi connection management

// Private namespace - these variables are only accessible within this file
namespace {

// How often to fetch wallet balance (1 minute = 60,000 milliseconds)
// UL = unsigned long (ensures the number is treated as the right type)
constexpr unsigned long KOIOS_INTERVAL_MS = 60UL * 1000UL;

// How often to fetch token/NFT data (10 minutes = 600,000 milliseconds)
// We fetch this less often because it's more data and takes longer
constexpr unsigned long PORTFOLIO_INTERVAL_MS = 10UL * 60UL * 1000UL;

// Maximum number of NFT policy IDs we can store
// Policy ID = unique identifier for an NFT collection
// Limited to 8 to match display capacity and API call limits
constexpr size_t MAX_POLICY_IDS = 8;

// Maximum number of tokens we can store (limited by screen display)
constexpr size_t MAX_TOKENS = 8;

// Maximum number of NFT collections we can store (limited by screen display)
constexpr size_t MAX_NFTS = 8;

// Global variables to store fetched data
// These persist between function calls (unlike local variables)

float walletBalance = 0.0f; // Your ADA balance (in ADA, not Lovelace)
int tokenCount = 0;         // How many different tokens you own
int nftCount = 0;           // How many different NFT collections you own

// Array to store Policy IDs (one per NFT collection)
// We need these to fetch floor prices from Cexplorer API
String policyIds[MAX_POLICY_IDS];
int policyIdCount = 0; // How many Policy IDs we've collected

// Arrays to store token and NFT data
// Arrays are like lists - we can store multiple items
TokenInfo tokens[MAX_TOKENS]; // Array of token information
NFTInfo nfts[MAX_NFTS];       // Array of NFT collection information

// Timestamps to track when we last fetched data
// Used to implement rate limiting (don't fetch too often)
unsigned long lastKoiosFetch = 0;     // When we last fetched wallet balance
unsigned long lastPortfolioFetch = 0; // When we last fetched tokens/NFTs

// Forward declarations - these functions are defined later in this file
// We declare them here so they can be called from other functions
void fetchWalletBalance(); // Fetches ADA balance from Koios
void fetchMinSwapData();   // Fetches tokens/NFTs from MinSwap
void fetchCexplorerData(const String &policyId); // Fetches NFT floor prices

} // namespace

/**
 * Initialize the data fetcher
 *
 * This function resets all data storage to zero/empty. It's called once at
 * startup to ensure we start with clean data.
 *
 * Think of it like clearing a whiteboard before starting a new lesson.
 */
void initDataFetcher() {
  // Reset all counters to zero
  walletBalance = 0.0f;
  tokenCount = 0;
  nftCount = 0;
  policyIdCount = 0;
  lastKoiosFetch = 0;
  lastPortfolioFetch = 0;

  // Clear all token data arrays
  // Loop through each position in the array and set it to empty/default values
  for (size_t i = 0; i < MAX_TOKENS; ++i) {
    tokens[i].ticker = "";      // Empty string
    tokens[i].amount = 0.0f;    // Zero amount
    tokens[i].value = 0.0f;     // Zero value
    tokens[i].change24h = 0.0f; // Zero change
  }

  // Clear all NFT data arrays
  for (size_t i = 0; i < MAX_NFTS; ++i) {
    nfts[i].name = "";         // Empty name
    nfts[i].amount = 0.0f;     // Zero amount
    nfts[i].floorPrice = 0.0f; // Zero floor price
    nfts[i].policyId = "";     // Empty policy ID
  }
}

/**
 * Update wallet balance data from Koios API
 *
 * This function implements rate limiting - it only fetches data if:
 * 1. WiFi is connected
 * 2. Enough time has passed since last fetch (1 minute)
 *
 * Rate limiting is important because:
 * - APIs have limits on how often you can request data
 * - Fetching too often wastes bandwidth and battery
 * - Wallet balance doesn't change that frequently anyway
 */
void updateKoiosData() {
  // Check if WiFi is connected - we can't fetch data without internet
  if (!wifiManagerIsConnected()) {
    return; // Exit early if no WiFi
  }

  // Get current time in milliseconds since device started
  const unsigned long now = millis();

  // Rate limiting check:
  // - If lastKoiosFetch is 0, we've never fetched (allow it)
  // - Otherwise, only fetch if at least 1 minute has passed
  if (lastKoiosFetch != 0 && (now - lastKoiosFetch) < KOIOS_INTERVAL_MS) {
    return; // Not enough time has passed, skip this update
  }

  // Record that we're fetching now
  lastKoiosFetch = now;

  // Actually fetch the wallet balance from Koios API
  fetchWalletBalance();
}

/**
 * Update token and NFT portfolio data
 *
 * This function fetches your token positions and NFT collections from MinSwap,
 * then fetches NFT floor prices from Cexplorer. It only runs every 10 minutes
 * because this data doesn't change as frequently and the API calls take longer.
 *
 * Process:
 * 1. Fetch tokens and NFTs from MinSwap API
 * 2. Extract Policy IDs from NFT data
 * 3. Fetch floor prices for each NFT collection from Cexplorer API
 */
void updatePortfolioData() {
  // Check WiFi connection first
  if (!wifiManagerIsConnected()) {
    return; // Can't fetch without internet
  }

  // Rate limiting - only fetch every 10 minutes
  const unsigned long now = millis();
  if (lastPortfolioFetch != 0 &&
      (now - lastPortfolioFetch) < PORTFOLIO_INTERVAL_MS) {
    return; // Not enough time has passed
  }

  // Record fetch time
  lastPortfolioFetch = now;

  // Step 1: Fetch tokens and NFTs from MinSwap
  // This populates the tokens[] and nfts[] arrays, and collects Policy IDs
  fetchMinSwapData();

  // Step 2: Fetch floor prices for each NFT collection from Cexplorer
  // We loop through all Policy IDs we collected and fetch floor price data
  // This gives us the "floor price" (lowest selling price) for each collection
  for (int i = 0; i < policyIdCount; ++i) {
    fetchCexplorerData(policyIds[i]);
  }
}

/**
 * Getter functions - These provide access to the stored data
 *
 * These are simple functions that return the values of our global variables.
 * Other files (like screen files) call these to get data for display.
 */

// Return your current ADA wallet balance
float getWalletBalance() { return walletBalance; }

// Return how many different tokens you own
int getTokenCount() { return tokenCount; }

// Return how many different NFT collections you own
int getNftCount() { return nftCount; }

// Return when wallet balance was last fetched (for "Last updated" display)
unsigned long getLastKoiosFetchTime() { return lastKoiosFetch; }

/**
 * Get information about a specific token
 *
 * @param index Which token to get (0 = first token, 1 = second, etc.)
 * @return TokenInfo structure, or empty structure if index is invalid
 *
 * Example: getToken(0) returns your first token, getToken(1) returns your
 * second
 */
TokenInfo getToken(int index) {
  // Create an empty token structure as default
  TokenInfo empty = {"", 0.0f, 0.0f, 0.0f};

  // Validate index - make sure it's within valid range
  // index must be >= 0 and < tokenCount
  if (index < 0 || index >= tokenCount) {
    return empty; // Invalid index, return empty structure
  }

  // Valid index, return the token data
  return tokens[index];
}

/**
 * Get information about a specific NFT collection
 *
 * @param index Which collection to get (0 = first collection, 1 = second, etc.)
 * @return NFTInfo structure, or empty structure if index is invalid
 */
NFTInfo getNFT(int index) {
  // Create an empty NFT structure as default
  NFTInfo empty = {"", 0.0f, 0.0f, ""};

  // Validate index
  if (index < 0 || index >= nftCount) {
    return empty; // Invalid index, return empty structure
  }

  // Valid index, return the NFT collection data
  return nfts[index];
}

namespace {

/**
 * Fetch wallet balance from Koios API
 *
 * Koios is a Cardano blockchain indexer - it provides fast access to blockchain
 * data without having to query the blockchain directly (which is slow).
 *
 * Process:
 * 1. Create HTTP client
 * 2. Build JSON request with your stake address
 * 3. Send POST request to Koios API
 * 4. Parse JSON response
 * 5. Extract balance (in Lovelace) and convert to ADA
 *
 * Important Cardano concepts:
 * - Stake Address: Your wallet's staking address (starts with "stake1...")
 * - Lovelace: The smallest unit of ADA (like cents to dollars)
 * - 1 ADA = 1,000,000 Lovelace
 * - We convert Lovelace to ADA for display
 */
void fetchWalletBalance() {
  Serial.println();
  Serial.println("--- Fetching Wallet Balance from Koios ---");

  // Create HTTP client object - this handles internet communication
  HTTPClient http;

  // Set the API endpoint URL (defined in config.h)
  http.begin(koiosApiUrl);

  // Tell the API we're sending JSON data
  http.addHeader("Content-Type", "application/json");

  // Build the JSON request payload
  // Koios API expects: {"_stake_addresses":["stake1..."]}
  // We're asking: "What's the balance for this stake address?"
  String jsonPayload = "{\"_stake_addresses\":[\"";
  jsonPayload += stakeAddress; // Your stake address from config.h
  jsonPayload += "\"]}";

  Serial.println("Sending POST request to Koios...");
  Serial.print("Payload: ");
  Serial.println(jsonPayload);

  // Send the HTTP POST request and get response code
  // POST means we're sending data (unlike GET which just requests data)
  int httpResponseCode = http.POST(jsonPayload);

  // Check if request was successful (response code > 0 means success)
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    // Get the response data (this is JSON text)
    String response = http.getString();

    // Create a JSON document to parse the response
    // 2048 = maximum size of JSON we expect (in bytes)
    DynamicJsonDocument doc(2048);

    // Parse the JSON string into a structured document we can access
    DeserializationError error = deserializeJson(doc, response);

    // Check if parsing was successful
    if (!error) {
      // Check if response is an array with at least one item
      if (doc.is<JsonArray>() && doc.size() > 0) {
        // Get the first (and only) account info object
        JsonObject accountInfo = doc[0];

        // Extract balance as a string (APIs often return large numbers as
        // strings)
        const char *balanceStr = accountInfo["total_balance"];
        long long balanceLovelace = 0;

        // Convert string to number (atoll = "ASCII to long long")
        if (balanceStr != nullptr) {
          balanceLovelace = atoll(balanceStr);
        }

        // Convert Lovelace to ADA
        // Example: 5,000,000 Lovelace / 1,000,000 = 5.0 ADA
        walletBalance = balanceLovelace / 1000000.0;

        // Print success message to Serial Monitor
        Serial.println();
        Serial.println("✓ Wallet Balance Fetched Successfully!");
        Serial.print("Stake Address: ");
        Serial.println(accountInfo["stake_address"].as<const char *>());
        Serial.print("Total Balance: ");
        Serial.print(walletBalance, 6); // Print with 6 decimal places
        Serial.println(" ADA");
      } else {
        Serial.println("Error: Empty response from Koios API");
      }
    } else {
      // JSON parsing failed - maybe API returned invalid JSON
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    // HTTP request failed (network error, timeout, etc.)
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
  }

  // Always close the HTTP connection when done
  http.end();
}

/**
 * Fetch token and NFT data from MinSwap API
 *
 * MinSwap is a DEX (Decentralized Exchange) on Cardano. Their API provides
 * portfolio information including:
 * - Token positions (what tokens you own and their values)
 * - NFT positions (what NFTs you own, grouped by collection)
 *
 * Process:
 * 1. Build URL with your wallet address as a parameter
 * 2. Send GET request (simpler than POST - just requesting data)
 * 3. Parse JSON response
 * 4. Extract tokens and store in tokens[] array
 * 5. Extract NFTs, group by Policy ID, and store in nfts[] array
 * 6. Collect Policy IDs for later floor price fetching
 */
void fetchMinSwapData() {
  Serial.println();
  Serial.println("--- Fetching Tokens and NFTs from MinSwap ---");

  // Create HTTP client
  HTTPClient http;

  // Build the API URL with query parameters
  // Query parameters are added after "?" in the URL
  // Example:
  // https://api.minswap.org/v1/portfolio/tokens?address=addr1...&only_minswap=true
  String fullUrl = String(minswapApiUrl);
  fullUrl += "?address=";
  fullUrl += walletAddress;               // Your wallet address from config.h
  fullUrl += "&only_minswap=true";        // Only show tokens from MinSwap
  fullUrl += "&filter_small_value=false"; // Don't filter out small value tokens

  Serial.print("Requesting: ");
  Serial.println(fullUrl);

  // Set the URL and send GET request
  // GET is simpler than POST - we're just requesting data, not sending data
  http.begin(fullUrl);
  Serial.println("Sending GET request to MinSwap...");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      Serial.println();
      Serial.println("✓ MinSwap Data Fetched Successfully!");

      // Check if response contains "positions" data
      if (doc.containsKey("positions")) {
        JsonObject positions = doc["positions"];

        // Process NFT positions first
        if (positions.containsKey("nft_positions")) {
          JsonArray nftArray = positions["nft_positions"];

          // Reset NFT storage before processing new data
          nftCount = 0;
          policyIdCount = 0;

          // Process each NFT position in the array
          // MinSwap returns each NFT as a separate entry, but we want to group
          // them by collection (Policy ID). So if you own 3 NFTs from the same
          // collection, we'll count them as one collection with amount = 3.
          for (int i = 0;
               i < nftArray.size() && nftCount < static_cast<int>(MAX_NFTS);
               ++i) {
            JsonObject nft = nftArray[i];

            // Get the Policy ID (also called "currency_symbol" in MinSwap API)
            // Policy ID is like a collection identifier - all NFTs from the
            // same collection have the same Policy ID
            const char *currencySymbol = nft["currency_symbol"];

            // Skip if no Policy ID (shouldn't happen, but safety check)
            if (currencySymbol == nullptr) {
              continue; // Skip to next NFT
            }

            String policyId = String(currencySymbol);

            // Extract NFT collection name from metadata
            // MinSwap provides metadata with the collection name
            String nftName = "Unknown NFT";
            if (nft.containsKey("asset")) {
              JsonObject assetInfo = nft["asset"];
              if (assetInfo.containsKey("metadata")) {
                JsonObject metadata = assetInfo["metadata"];
                // The "|" operator means "use this value, or if missing, use
                // default"
                nftName = metadata["name"] | "Unknown NFT";
              }
            }

            // Check if we already have this Policy ID in our array
            // We want to group NFTs by collection, so we check if we've seen
            // this Policy ID before
            int existingIndex = -1;
            for (int j = 0; j < nftCount; ++j) {
              if (nfts[j].policyId == policyId) {
                existingIndex = j; // Found it! Remember which position
                break;
              }
            }

            if (existingIndex >= 0) {
              // We already have this collection - just increment the count
              // Example: If you own 2 Cardano Punks, then find a 3rd one,
              // we increment amount from 2 to 3
              nfts[existingIndex].amount += 1.0f;
            } else {
              // New collection we haven't seen before - add it to our array
              nfts[nftCount].name = nftName;
              nfts[nftCount].amount = 1.0f; // First NFT from this collection
              nfts[nftCount].floorPrice =
                  0.0f; // Will be updated by Cexplorer later
              nfts[nftCount].policyId = policyId;

              // Save Policy ID so we can fetch floor price from Cexplorer
              if (policyIdCount < static_cast<int>(MAX_POLICY_IDS)) {
                policyIds[policyIdCount] = policyId;
                ++policyIdCount;
              }

              Serial.print("  NFT Collection ");
              Serial.print(nftCount + 1);
              Serial.print(": ");
              Serial.print(nftName);
              Serial.print(" (Policy ID: ");
              Serial.print(currencySymbol);
              Serial.println(")");

              ++nftCount; // Move to next position in array
            }
          }

          Serial.print("NFT Collections found: ");
          Serial.println(nftCount);
          Serial.print("Extracted ");
          Serial.print(policyIdCount);
          Serial.println(" policy ID(s) for Cexplorer API calls");
        }

        // Process token positions (regular tokens, not NFTs)
        if (positions.containsKey("asset_positions")) {
          JsonArray assetArray = positions["asset_positions"];

          // Count how many tokens we found
          tokenCount = assetArray.size();

          // Limit to maximum we can display (8 tokens)
          if (tokenCount > static_cast<int>(MAX_TOKENS)) {
            tokenCount = MAX_TOKENS;
          }
          Serial.print("Tokens found: ");
          Serial.println(tokenCount);

          // Process each token
          for (int i = 0;
               i < assetArray.size() && i < static_cast<int>(MAX_TOKENS); ++i) {
            JsonObject asset = assetArray[i];

            // Check if token has required data
            if (asset.containsKey("asset")) {
              JsonObject assetInfo = asset["asset"];
              if (assetInfo.containsKey("metadata")) {
                JsonObject metadata = assetInfo["metadata"];

                // Extract token information from JSON
                // The "|" operator provides default values if data is missing
                String ticker = metadata["ticker"] |
                                "UNKNOWN"; // Token symbol (e.g., "MIN")
                String name = metadata["name"] | "Unknown Token"; // Full name
                float priceUsd =
                    asset["price_usd"] | 0.0f;         // Price per token in USD
                float amount = asset["amount"] | 0.0f; // How many you own
                float change24h =
                    asset["pnl_24h_percent"] | 0.0f; // 24h price change %

                // Store token data in our array
                tokens[i].ticker = ticker;
                tokens[i].amount = amount;
                tokens[i].value =
                    priceUsd * amount; // Total value = price × amount
                tokens[i].change24h = change24h;

                Serial.print("  Token ");
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(ticker);
                Serial.print(" (");
                Serial.print(name);
                Serial.print(") - Price: $");
                Serial.print(priceUsd, 4);
                Serial.print(", Amount: ");
                Serial.print(amount, 2);
                Serial.print(", 24h Change: ");
                Serial.print(change24h, 2);
                Serial.println("%");
              }
            }
          }
        }
      } else {
        Serial.println("Warning: No positions found in MinSwap response");
      }
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

/**
 * Fetch NFT collection information from Cexplorer API
 *
 * Cexplorer is a Cardano blockchain explorer that provides detailed information
 * about NFT collections, including:
 * - Collection name
 * - Floor price (lowest current selling price)
 * - Number of owners
 * - Other statistics
 *
 * This function is called once for each NFT Policy ID we found from MinSwap.
 *
 * @param policyId The Policy ID of the NFT collection to look up
 *
 * Process:
 * 1. Build URL with Policy ID as query parameter
 * 2. Send GET request to Cexplorer API
 * 3. Parse JSON response
 * 4. Extract collection name and floor price
 * 5. Update the corresponding NFT entry in our nfts[] array
 */
void fetchCexplorerData(const String &policyId) {
  Serial.println();
  Serial.println("--- Fetching NFT Info from Cexplorer ---");
  Serial.print("Policy ID: ");
  Serial.println(policyId);

  // Create HTTP client
  HTTPClient http;

  // Build URL with Policy ID as query parameter
  // Example: https://api.cexplorer.io/v1/policy/detail?id=f0ff48bbb7...
  String fullUrl = String(cexplorerApiUrl);
  fullUrl += "?id=";
  fullUrl += policyId;

  Serial.print("Requesting: ");
  Serial.println(fullUrl);

  // Set URL and prepare request
  http.begin(fullUrl);

  // Optional: Add API key header if you have one
  // Some APIs require authentication, but Cexplorer works without it
  // http.addHeader("api-key", cexplorerApiKey);

  Serial.println("Sending GET request to Cexplorer...");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      Serial.println();
      Serial.println("✓ Cexplorer Data Fetched Successfully!");

      // Check if response contains "data" object
      if (doc.containsKey("data")) {
        JsonObject data = doc["data"];

        // Check if collection information is available
        if (data.containsKey("collection")) {
          JsonObject collection = data["collection"];

          // Extract collection name
          // Cexplorer usually has better/more accurate names than MinSwap
          String collectionName = collection["name"] | "Unknown";

          Serial.print("Collection Name: ");
          Serial.println(collectionName);

          // Extract floor price (lowest current selling price)
          float floorPriceAda = 0.0f;
          if (collection.containsKey("stats")) {
            JsonObject stats = collection["stats"];

            // Floor price comes in Lovelace (smallest ADA unit)
            long floorLovelace = stats["floor"] | 0;

            // Convert to ADA (divide by 1,000,000)
            floorPriceAda = floorLovelace / 1000000.0f;

            // Also get number of owners (for debugging/logging)
            int owners = stats["owners"] | 0;

            Serial.print("Floor Price: ");
            Serial.print(floorPriceAda, 2); // Print with 2 decimal places
            Serial.println(" ADA");
            Serial.print("Owners: ");
            Serial.println(owners);
          }

          // Now update our NFT array with the collection name and floor price
          // We need to find which NFT entry has this Policy ID
          for (int i = 0; i < nftCount && i < static_cast<int>(MAX_NFTS); ++i) {
            if (nfts[i].policyId == policyId) {
              // Found the matching NFT collection!
              // Update with better name from Cexplorer (more accurate than
              // MinSwap)
              nfts[i].name = collectionName;

              // Update floor price if we got one
              if (floorPriceAda > 0.0f) {
                nfts[i].floorPrice = floorPriceAda;
              }

              break; // Found it, no need to keep searching
            }
          }
        }
      } else {
        Serial.println("Warning: No data found in Cexplorer response");
      }
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 401) {
      Serial.println("Error: 401 Unauthorized - Check your Cexplorer API key!");
    }
  }

  http.end();
}

} // namespace
