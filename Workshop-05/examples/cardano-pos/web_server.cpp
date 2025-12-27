#include "web_server.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>

namespace {
WebServer server(80);       // Web server on port 80
bool serverStarted = false; // Flag to check if server is started
const char *TRANSACTIONS_FILE = "/transactions.json";

// Callback for new transaction notifications
TransactionCallback transactionCallback = nullptr;
TFT_eSPI* displayPtr = nullptr;

// Get MIME type based on file extension
String getContentType(String filename) {
  if (filename.endsWith(".html") || filename.endsWith("/")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".json")) {
    return "application/json";
  } else {
    return "text/plain";
  }
}

// Handle GET /api/transactions - serve transactions JSON file
void handleGetTransactions() {
  Serial.println("GET /api/transactions");

  // Check if transactions file exists
  if (!LittleFS.exists(TRANSACTIONS_FILE)) {
    // Return empty array if file doesn't exist
    server.send(200, "application/json", "[]");
    Serial.println("Transactions file not found, returning empty array");
    return;
  }

  // Read and serve the file
  File file = LittleFS.open(TRANSACTIONS_FILE, "r");
  if (file) {
    server.streamFile(file, "application/json");
    file.close();
    Serial.println("Served transactions.json");
  } else {
    server.send(500, "application/json",
                "{\"error\":\"Error opening transactions file\"}");
    Serial.println("Error opening transactions file");
  }
}

// Handle POST /api/transactions - add a new transaction
void handlePostTransactions() {
  Serial.println("POST /api/transactions");

  // Check if request has body
  if (!server.hasArg("plain")) {
    server.send(400, "application/json",
                "{\"error\":\"Missing request body\"}");
    Serial.println("POST request missing body");
    return;
  }

  String body = server.arg("plain");
  Serial.print("Request body: ");
  Serial.println(body);

  // Parse the request body to get amount
  DynamicJsonDocument requestDoc(1024);
  DeserializationError error = deserializeJson(requestDoc, body);

  if (error) {
    server.send(400, "application/json",
                "{\"error\":\"Invalid JSON in request body\"}");
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  if (!requestDoc.containsKey("amount")) {
    server.send(400, "application/json",
                "{\"error\":\"Missing 'amount' field\"}");
    Serial.println("Missing 'amount' field");
    return;
  }

  if (!requestDoc.containsKey("timestamp")) {
    server.send(400, "application/json",
                "{\"error\":\"Missing 'timestamp' field\"}");
    Serial.println("Missing 'timestamp' field");
    return;
  }

  // Use uint64_t to handle large lovelace amounts (e.g., 15000 ADA = 15 billion
  // lovelace)
  uint64_t amount = requestDoc["amount"].as<uint64_t>();
  // Use uint64_t to handle large JavaScript timestamps (milliseconds since
  // epoch)
  uint64_t timestamp = requestDoc["timestamp"].as<uint64_t>();

  // Read existing transactions
  DynamicJsonDocument transactionsDoc(4096);
  JsonArray transactions = transactionsDoc.to<JsonArray>();

  if (LittleFS.exists(TRANSACTIONS_FILE)) {
    File file = LittleFS.open(TRANSACTIONS_FILE, "r");
    if (file) {
      DeserializationError error = deserializeJson(transactionsDoc, file);
      file.close();

      if (error) {
        Serial.print("Error parsing existing transactions: ");
        Serial.println(error.c_str());
        // Continue with empty array if parse fails
        transactions = transactionsDoc.to<JsonArray>();
      } else {
        transactions = transactionsDoc.as<JsonArray>();
      }
    }
  }

  // Find the highest ID to auto-increment
  int maxId = 0;
  for (JsonObject transaction : transactions) {
    if (transaction.containsKey("id")) {
      int id = transaction["id"];
      if (id > maxId) {
        maxId = id;
      }
    }
  }

  // Calculate new transaction ID
  int newId = maxId + 1;

  // Create new transaction
  JsonObject newTransaction = transactions.createNestedObject();
  newTransaction["id"] = newId;
  newTransaction["timestamp"] = timestamp;
  // Add the transaction ID to the amount (amount + id)
  newTransaction["amount"] = amount + newId;
  newTransaction["txHash"] = ""; // Empty transaction hash field

  // Write back to file
  File file = LittleFS.open(TRANSACTIONS_FILE, "w");
  if (file) {
    serializeJson(transactionsDoc, file);
    file.close();

    // Return the new transaction
    String response;
    serializeJson(newTransaction, response);
    server.send(201, "application/json", response);
    Serial.print("Added transaction with ID: ");
    Serial.print(newId);
    Serial.print(", Amount (with ID): ");
    Serial.println(amount + newId);
    
    // Notify callback about new transaction (if set)
    if (transactionCallback != nullptr && displayPtr != nullptr) {
      transactionCallback(displayPtr, newId, amount + newId);
    }
  } else {
    server.send(500, "application/json",
                "{\"error\":\"Error writing transactions file\"}");
    Serial.println("Error writing transactions file");
  }
}

// Handle file requests
void handleFileRequest() {
  String path = server.uri();

  // Default to index.html for root path
  if (path == "/" || path == "") {
    path = "/index.html";
  }

  // Ensure path starts with /
  if (!path.startsWith("/")) {
    path = "/" + path;
  }

  // Check if file exists in LittleFS
  if (LittleFS.exists(path)) {
    String contentType = getContentType(path);
    File file = LittleFS.open(path, "r");
    if (file) {
      server.streamFile(file, contentType);
      file.close();
      Serial.print("Served file: ");
      Serial.println(path);
    } else {
      server.send(500, "text/plain", "Error opening file");
      Serial.print("Error opening file: ");
      Serial.println(path);
    }
  } else {
    // File not found - try index.html as fallback
    if (path != "/index.html" && LittleFS.exists("/index.html")) {
      File file = LittleFS.open("/index.html", "r");
      if (file) {
        server.streamFile(file, "text/html");
        file.close();
        Serial.print("File not found, serving index.html: ");
        Serial.println(path);
      } else {
        server.send(404, "text/plain", "File not found");
      }
    } else {
      // 404 Not Found
      server.send(404, "text/plain", "File not found");
      Serial.print("404 - File not found: ");
      Serial.println(path);
    }
  }
}
} // namespace

void setTransactionCreatedCallback(TransactionCallback callback, TFT_eSPI* display) {
  transactionCallback = callback;
  displayPtr = display;
}

void webServerSetup() {
  // Initialize LittleFS file system
  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: LittleFS Mount Failed");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  // List all files in LittleFS (for debugging)
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  Serial.println("Files in LittleFS:");
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" (");
    Serial.print(file.size());
    Serial.println(" bytes)");
    file = root.openNextFile();
  }

  // Register API endpoints
  server.on("/api/transactions", HTTP_GET, handleGetTransactions);
  server.on("/api/transactions", HTTP_POST, handlePostTransactions);

  // Serve files from root and all subdirectories (must be last)
  server.onNotFound(handleFileRequest);

  // Start the server
  server.begin();
  serverStarted = true;

  // Print the server's IP address
  Serial.print("Web server started on http://");
  Serial.println(WiFi.localIP());
}

// Function to handle incoming client requests
void webServerLoop() {
  // If the server is started, handle incoming client requests
  if (serverStarted) {
    server.handleClient();
  }
}

// Function to check if the server is running
bool webServerIsRunning() { return serverStarted; }
