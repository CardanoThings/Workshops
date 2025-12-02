/**
 * data_fetcher.h - Header file for blockchain data fetching
 * 
 * This file defines the data structures and functions used to fetch and store
 * information from Cardano blockchain APIs.
 * 
 * What is a header file (.h)?
 * - It declares what functions and structures exist, but doesn't implement them
 * - The actual code is in data_fetcher.cpp
 * - Other files can #include this to use these functions
 */

#ifndef DATA_FETCHER_H
#define DATA_FETCHER_H

#include <Arduino.h>

/**
 * TokenInfo - Structure to store information about a Cardano token
 * 
 * In Cardano, tokens are custom assets (like cryptocurrencies) that can be
 * created and traded. Examples: MIN (MinSwap token), HOSKY (meme token), etc.
 * 
 * This structure holds all the information we need to display about a token.
 */
struct TokenInfo {
  String ticker;      // Short symbol for the token (e.g., "MIN", "ADA")
  float amount;       // How many tokens you own
  float value;        // Total value of your tokens in USD (amount × price)
  float change24h;    // Price change percentage over last 24 hours (can be negative)
};

/**
 * NFTInfo - Structure to store information about an NFT collection
 * 
 * NFT = Non-Fungible Token (unique digital collectible)
 * In Cardano, NFTs are grouped by "Policy ID" - think of it as the collection ID.
 * All NFTs from the same collection share the same Policy ID.
 * 
 * Example: If you own 3 "Cardano Punks" NFTs, they all have the same Policy ID,
 * but each individual NFT is unique.
 */
struct NFTInfo {
  String name;        // Name of the NFT collection (e.g., "Cardano Punks")
  float amount;       // Number of NFTs you own from this collection
  float floorPrice;   // Floor price = lowest price this collection is selling for (in ADA)
  String policyId;    // Policy ID = unique identifier for this NFT collection
                      // Used to match NFTs with their floor price data
};

// Function declarations - these are implemented in data_fetcher.cpp

/**
 * Initialize the data fetcher
 * Sets all counters and arrays to zero/empty
 */
void initDataFetcher();

/**
 * Update wallet balance from Koios API
 * Fetches your ADA balance every minute (if enough time has passed)
 * Koios is a Cardano blockchain indexer - it provides fast access to blockchain data
 */
void updateKoiosData();

/**
 * Update token and NFT data from MinSwap and Cexplorer APIs
 * Fetches your token positions and NFT collections every 10 minutes
 * MinSwap is a DEX (Decentralized Exchange) that provides portfolio data
 * Cexplorer provides NFT collection information and floor prices
 */
void updatePortfolioData();

// Getter functions - these return the stored data

/**
 * Get your current ADA wallet balance
 * @return Balance in ADA (1 ADA = 1,000,000 Lovelace)
 */
float getWalletBalance();

/**
 * Get the number of different tokens you own
 * @return Number of unique tokens (max 8)
 */
int getTokenCount();

/**
 * Get the number of different NFT collections you own
 * @return Number of unique NFT collections (max 8)
 */
int getNftCount();

/**
 * Get timestamp of when wallet balance was last fetched
 * Useful for displaying "Last updated: X minutes ago"
 * @return Timestamp in milliseconds, or 0 if never fetched
 */
unsigned long getLastKoiosFetchTime();

/**
 * Get information about a specific token
 * @param index Which token to get (0 = first token, 1 = second, etc.)
 * @return TokenInfo structure with token data, or empty if index is invalid
 */
TokenInfo getToken(int index);

/**
 * Get information about a specific NFT collection
 * @param index Which collection to get (0 = first collection, 1 = second, etc.)
 * @return NFTInfo structure with NFT data, or empty if index is invalid
 */
NFTInfo getNFT(int index);

#endif
