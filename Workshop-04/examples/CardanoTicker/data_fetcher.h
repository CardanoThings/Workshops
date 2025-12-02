#ifndef DATA_FETCHER_H
#define DATA_FETCHER_H

#include <Arduino.h>

struct TokenInfo {
  String ticker;
  float amount;
  float value;      // Total value in USD
  float change24h;  // 24h change percentage
};

void initDataFetcher();
void updateKoiosData();     // Fetch wallet balance every minute
void updatePortfolioData(); // Fetch tokens/NFTs every 10 minutes

float getWalletBalance();
int getTokenCount();
int getNftCount();
unsigned long getLastKoiosFetchTime(); // Returns timestamp of last Koios fetch
                                       // (0 if never fetched)
TokenInfo getToken(int index);         // Get token info at index (0-based)

#endif
