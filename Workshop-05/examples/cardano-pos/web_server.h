#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

// Forward declaration
class TFT_eSPI;

// Callback function type for new transaction notifications
// transactionId: ID of the transaction
// lovelaceAmount: Amount in lovelace (with ID already added)
typedef void (*TransactionCallback)(TFT_eSPI* display, int transactionId, uint64_t lovelaceAmount);

// Set callback for when a new transaction is created
void setTransactionCreatedCallback(TransactionCallback callback, TFT_eSPI* display);

// Initialize the web server (call after WiFi is connected)
void webServerSetup();

// Handle server requests (call in loop())
void webServerLoop();

// Check if server is running
bool webServerIsRunning();

#endif



