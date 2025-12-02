/**
 * config.h - Configuration header file
 * 
 * This file declares external variables that are defined in config.cpp.
 * These contain your wallet addresses and API endpoints.
 * 
 * Important: You need to edit config.cpp with your actual addresses!
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Your Cardano stake address (starts with "stake1..." on mainnet)
// This is used to fetch your wallet balance from Koios API
extern String stakeAddress;

// Your Cardano wallet address (starts with "addr1..." on mainnet)
// This is used to fetch your token and NFT positions from MinSwap API
extern String walletAddress;

// API endpoint URLs
// These point to the Cardano blockchain APIs we use to fetch data
extern const char *koiosApiUrl;      // Koios API - for wallet balance
extern const char *minswapApiUrl;    // MinSwap API - for tokens and NFTs
extern const char *cexplorerApiUrl;  // Cexplorer API - for NFT floor prices

#endif
