/**
 * config.cpp - Configuration implementation file
 *
 * This file defines the actual values for wallet addresses and API endpoints
 * that are declared in config.h. Edit these values with your own addresses
 * and API keys before uploading to your device.
 */

#include "config.h"

// Your Cardano stake address (starts with "stake1..." on mainnet)
// Used to fetch your ADA wallet balance from the Koios API
// Replace this with your own stake address
String stakeAddress =
    "stake1u8l0y82je0t2wkkpps97rv0q7lf882q0fc24gwjz9nacz0c5gt5k"
    "3";

// Your Cardano wallet address (starts with "addr1..." on mainnet)
// Used to fetch your token and NFT positions from the MinSwap API
// Replace this with your own wallet address
String walletAddress =
    "addr1q8xy5cfmccecvvr2z7ns7mzld8qkq73lgwnq7vy3my0s5rl77gw49j7k5advzrqtuxc7p"
    "a7jww5q7ns42sayyt8msylsx4k2qx";

// Cexplorer API key for accessing NFT floor price data
// Get your free API key from: https://cexplorer.io/api
// Replace "your-api-key-here" with your actual API key
String cexplorerApiKey = "your-api-key-here";

// API endpoint URLs
// These point to Cardano blockchain APIs used to fetch wallet data

// Koios API endpoint - fetches wallet balance (ADA)
// Koios is a Cardano blockchain indexer that provides fast access to blockchain
// data
const char *koiosApiUrl = "https://api.koios.rest/api/v1/account_info";

// MinSwap API endpoint - fetches token and NFT portfolio data
// MinSwap is a decentralized exchange (DEX) that provides portfolio information
const char *minswapApiUrl =
    "https://monorepo-mainnet-prod.minswap.org/v1/portfolio/tokens";

// Cexplorer API endpoint - fetches NFT collection floor prices
// Cexplorer provides detailed NFT collection information including floor prices
const char *cexplorerApiUrl =
    "https://api-mainnet-stage.cexplorer.io/v1/policy/detail";
