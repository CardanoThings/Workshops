#ifndef TICKER_H
#define TICKER_H

#include <Arduino.h>

// Data structures for tokens and NFTs
struct TokenData {
  String ticker;
  float price;
  float change24h;
};

// Initialize the ticker display
void initTicker();

// Update the ticker display (call this in loop)
void updateTicker();

#endif
