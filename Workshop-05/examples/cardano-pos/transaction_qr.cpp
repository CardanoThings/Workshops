#include "transaction_qr.h"
#include "secrets.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <qrcode_espi.h>

namespace {
TFT_eSprite *qrSprite = nullptr;
QRcode_eSPI *qrcode = nullptr;
const char *TRANSACTIONS_FILE = "/transactions.json";
unsigned long lastCheckTime = 0;
const unsigned long CHECK_INTERVAL = 10000; // Check every 10 seconds
unsigned long waitingStartTime = 0;
bool isWaitingForPayment = false;
int waitingTransactionId = -1;
uint64_t waitingLovelaceAmount = 0;
bool waitingScreenDrawn = false;

const char *KOIOS_API_URL = "https://preprod.koios.rest/api/v1/address_utxos";
} // namespace

void transactionQRInit(TFT_eSPI &display) {
  int qrSize = min(display.width(), display.height()) - 20;
  qrSprite = new TFT_eSprite(&display);
  qrSprite->createSprite(qrSize, qrSize);
  qrSprite->fillSprite(TFT_WHITE);
  qrcode = new QRcode_eSPI(qrSprite);
  qrcode->init();

  lastCheckTime = 0;
  waitingStartTime = 0;
  isWaitingForPayment = false;
  waitingTransactionId = -1;
  waitingLovelaceAmount = 0;
  waitingScreenDrawn = false;
}

// Helper: Update transaction hash in LittleFS
bool updateTransactionHash(int transactionId, const String &txHash) {
  if (!LittleFS.exists(TRANSACTIONS_FILE)) {
    return false;
  }

  File file = LittleFS.open(TRANSACTIONS_FILE, "r");
  if (!file) {
    return false;
  }

  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    return false;
  }

  JsonArray transactions = doc.as<JsonArray>();
  bool updated = false;

  for (JsonObject tx : transactions) {
    if (tx.containsKey("id") && tx["id"] == transactionId) {
      tx["txHash"] = txHash;
      updated = true;
      break;
    }
  }

  if (updated) {
    file = LittleFS.open(TRANSACTIONS_FILE, "w");
    if (file) {
      serializeJson(doc, file);
      file.close();
      return true;
    }
  }

  return false;
}

// Function 1: Display QR code for a transaction
void displayTransactionQR(TFT_eSPI &display, int transactionId,
                          uint64_t lovelaceAmount) {
  // Calculate original amount (subtract ID) for display
  uint64_t originalAmount = lovelaceAmount - transactionId;
  float adaAmountForDisplay = (float)originalAmount / 1000000.0;

  // For QR code, use full amount including ID (lovelaceAmount already has ID
  // added)
  float adaAmountForQR = (float)lovelaceAmount / 1000000.0;

  // Build QR code URL: web+cardano:[paymentAddress]?amount=[ADAamount with ID]
  String qrContent = "web+cardano:";
  qrContent += PAYMENT_ADDRESS;
  qrContent += "?amount=";
  qrContent += String(adaAmountForQR, 6);

  // Generate and display QR code
  qrSprite->fillSprite(TFT_WHITE);
  qrcode->create(qrContent);
  display.fillScreen(TFT_BLACK);

  int spriteX = (display.width() - qrSprite->width()) / 2;
  int spriteY = (display.height() - qrSprite->height()) / 2;
  qrSprite->pushSprite(spriteX, spriteY);

  // Display transaction info
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.setTextDatum(TC_DATUM);
  display.drawString("TX ID: " + String(transactionId), display.width() / 2,
                     spriteY + qrSprite->height() + 5);
  display.drawString(String(adaAmountForDisplay, 2) + " ADA",
                     display.width() / 2, spriteY + qrSprite->height() + 20);
}

// Function 2: Check for transaction using Koios API
// transactionId: ID of the transaction
// lovelaceAmount: Amount in lovelace (with ID already added)
// Returns transaction hash if payment received, empty string otherwise
String checkForTransaction(int transactionId, uint64_t lovelaceAmount) {
  if (!WiFi.isConnected()) {
    Serial.println("[Transaction Check] WiFi not connected, skipping check");
    return "";
  }

  Serial.print("[Transaction Check] Checking for payment - TX ID: ");
  Serial.print(transactionId);
  Serial.print(lovelaceAmount);
  Serial.println(" lovelace)");

  // Build API URL with amount filter
  String url = String(KOIOS_API_URL);
  url += "?value=eq.";
  url += String(lovelaceAmount);

  // Make POST request
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Build JSON request body
  DynamicJsonDocument requestDoc(512);
  JsonArray addresses = requestDoc.createNestedArray("_addresses");
  addresses.add(PAYMENT_ADDRESS);

  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.print("[Transaction Check] Sending request to: ");
  Serial.println(url);
  Serial.print("[Transaction Check] Request body: ");
  Serial.println(requestBody);

  int httpCode = http.POST(requestBody);
  Serial.print("[Transaction Check] HTTP response code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument responseDoc(2048);
    DeserializationError error = deserializeJson(responseDoc, payload);
    http.end();

    if (!error && responseDoc.is<JsonArray>()) {
      JsonArray utxos = responseDoc.as<JsonArray>();
      Serial.print("[Transaction Check] Found ");
      Serial.print(utxos.size());
      Serial.println(" UTXO(s)");

      if (utxos.size() > 0 && utxos[0].containsKey("tx_hash")) {
        String txHash = utxos[0]["tx_hash"].as<String>();
        Serial.print("[Transaction Check] Payment found! Transaction hash: ");
        Serial.println(txHash);
        return txHash;
      } else {
        Serial.println(
            "[Transaction Check] No transaction hash in UTXO response");
      }
    } else {
      Serial.print("[Transaction Check] JSON parsing error: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("[Transaction Check] HTTP error, response: ");
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.println("Connection failed");
    }
  }

  http.end();
  Serial.println("[Transaction Check] No payment found yet");
  return "";
}

// Function 3: Display success message and update transaction JSON with hash
void displaySuccessAndUpdateHash(TFT_eSPI &display, int transactionId,
                                 const String &txHash) {
  // Update transaction with hash
  updateTransactionHash(transactionId, txHash);

  // Display success message
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(2);
  display.setTextDatum(MC_DATUM);
  display.drawString("Payment Received!", display.width() / 2,
                     display.height() / 2);

  Serial.print("Payment received! Transaction hash: ");
  Serial.println(txHash);
}

// Display QR code with call to action
void displayWaitingMessage(TFT_eSPI &display, int transactionId,
                           uint64_t lovelaceAmount, bool initialDraw) {
  // Calculate original amount (subtract ID) for display
  uint64_t originalAmount = lovelaceAmount - transactionId;
  float adaAmountForDisplay = (float)originalAmount / 1000000.0;

  // For QR code, use full amount including ID (lovelaceAmount already has ID
  // added)
  float adaAmountForQR = (float)lovelaceAmount / 1000000.0;

  // Only draw static elements on initial draw
  if (initialDraw) {
    // White background
    display.fillScreen(TFT_WHITE);

    // Build QR code URL (use amount with ID included)
    String qrContent = "web+cardano:";
    qrContent += PAYMENT_ADDRESS;
    qrContent += "?amount=";
    qrContent += String(adaAmountForQR, 8);

    Serial.print("[Transaction Check] QR content: ");
    Serial.println(qrContent);

    // Generate QR code on sprite (white background, black QR code)
    qrSprite->fillSprite(TFT_WHITE);
    qrcode->create(qrContent);

    // Position QR code below the text
    int spriteX = (display.width() - qrSprite->width()) / 2;
    int spriteY = 30; // Below the "Waiting for Payment" text
    qrSprite->pushSprite(spriteX, spriteY);

    // Display "Waiting for Payment" text above QR code (black text on white,
    // smaller size)
    display.setTextColor(TFT_BLACK);
    display.setTextSize(2);
    display.setTextDatum(TC_DATUM);
    display.drawString("PLEASE PAY NOW!", display.width() / 2, 20);

    // Display transaction ID and ADA amount below QR code
    int infoY = spriteY + qrSprite->height() + 5;
    display.setTextSize(1);
    display.setTextColor(TFT_BLACK);

    // TX ID left-aligned with 25px padding from left edge of QR code
    display.setTextDatum(TL_DATUM); // Top Left datum
    String txInfo = "TX ID: " + String(transactionId);
    display.drawString(txInfo, spriteX + 25, infoY);

    // ADA amount right-aligned with 25px padding from right edge of QR code
    display.setTextDatum(TR_DATUM); // Top Right datum
    String adaInfo = String(adaAmountForDisplay, 2) + " ADA";
    display.drawString(adaInfo, spriteX + qrSprite->width() - 25, infoY);
  }
}

void displayNewTransactionQR(TFT_eSPI *display, int transactionId,
                             uint64_t lovelaceAmount) {
  if (display == nullptr) {
    return;
  }
  waitingStartTime = millis();
  isWaitingForPayment = true;
  waitingTransactionId = transactionId;
  waitingLovelaceAmount = lovelaceAmount;
  waitingScreenDrawn = false;
  lastCheckTime = millis();

  uint64_t originalAmount = lovelaceAmount - transactionId;
  float adaAmount = (float)originalAmount / 1000000.0;

  Serial.println("========================================");
  Serial.println("[Transaction Listener] Starting to listen for payment");
  Serial.print("  Transaction ID: ");
  Serial.println(transactionId);
  Serial.print("  Amount: ");
  Serial.print(adaAmount, 6);
  Serial.print(" ADA (");
  Serial.print(originalAmount);
  Serial.println(" lovelace)");
  Serial.print("  Payment Address: ");
  Serial.println(PAYMENT_ADDRESS);
  Serial.print("  Check interval: ");
  Serial.print(CHECK_INTERVAL / 1000);
  Serial.println(" seconds");
  Serial.println("========================================");

  // Draw initial waiting screen
  displayWaitingMessage(*display, transactionId, lovelaceAmount, true);
  waitingScreenDrawn = true;
}

void transactionQRUpdate(TFT_eSPI &display) {
  unsigned long currentTime = millis();

  // If waiting for payment, check for payment
  if (isWaitingForPayment && waitingTransactionId != -1) {
    unsigned long waitTimeSeconds = (currentTime - waitingStartTime) / 1000;

    // Check for payment every CHECK_INTERVAL
    if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
      lastCheckTime = currentTime;
      Serial.print("[Transaction Listener] Checking payment (waiting for ");
      Serial.print(waitTimeSeconds);
      Serial.println(" seconds)...");

      String txHash =
          checkForTransaction(waitingTransactionId, waitingLovelaceAmount);
      if (txHash.length() > 0) {
        Serial.println(
            "[Transaction Listener] Payment confirmed! Stopping listener.");
        displaySuccessAndUpdateHash(display, waitingTransactionId, txHash);
        isWaitingForPayment = false;
        waitingTransactionId = -1;
        waitingLovelaceAmount = 0;
      } else {
        Serial.println("[Transaction Listener] Payment not found, will check "
                       "again in 10 seconds");
      }
    }
  }
}

// Legacy function - kept for compatibility but not used
bool checkTransactionOnChain(int transactionId) { return false; }
