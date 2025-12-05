#ifndef TRANSACTION_DISPLAY_H
#define TRANSACTION_DISPLAY_H

#include <Arduino.h>

// Function to display transaction details on the TFT screen
// amount: amount in lovelace
// address: Cardano address (will be truncated for display)
// timestamp: Unix timestamp
void displayTransaction(unsigned long amount, const char* address, unsigned long timestamp);

#endif

