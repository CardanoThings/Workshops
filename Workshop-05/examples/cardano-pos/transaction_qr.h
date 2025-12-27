#ifndef TRANSACTION_QR_H
#define TRANSACTION_QR_H

#include <Arduino.h>

// Forward declaration
class TFT_eSPI;

// Transaction structure
struct Transaction {
  int id;
  uint64_t amount; // Amount in lovelace (with ID added)
  uint64_t timestamp;
  String txHash; // Transaction hash (empty until confirmed)
};

// Initialize the transaction QR display system
void transactionQRInit(TFT_eSPI &display);

// Display QR code for a transaction
// transactionId: ID of the transaction
// lovelaceAmount: Amount in lovelace (with ID already added)
void displayTransactionQR(TFT_eSPI &display, int transactionId,
                          uint64_t lovelaceAmount);

// Display QR code for newly created transaction (called immediately after
// creation) transactionId: ID of the transaction lovelaceAmount: Amount in
// lovelace (with ID already added)
void displayNewTransactionQR(TFT_eSPI *display, int transactionId,
                             uint64_t lovelaceAmount);

// Check on-chain status of a transaction
// transactionId: ID of the transaction to check
// Returns true if transaction is confirmed on-chain
bool checkTransactionOnChain(int transactionId);

// Update function to be called in loop() - checks on-chain status only
// QR codes are only displayed when triggered by POST request callback
void transactionQRUpdate(TFT_eSPI &display);

#endif
