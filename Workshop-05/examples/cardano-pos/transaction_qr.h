#ifndef TRANSACTION_QR_H
#define TRANSACTION_QR_H

#include <Arduino.h>

// Forward declaration
class TFT_eSPI;

// Initialize the transaction QR display system
void transactionQRInit(TFT_eSPI &display);

// Display QR code for newly created transaction (called immediately after
// creation)
// transactionId: ID of the transaction
// lovelaceAmount: Amount in lovelace (with ID already added)
void displayNewTransactionQR(TFT_eSPI *display, int transactionId,
                             uint64_t lovelaceAmount);

// Update function to be called in loop() - checks on-chain status and manages
// display states
void transactionQRUpdate(TFT_eSPI &display);

#endif
