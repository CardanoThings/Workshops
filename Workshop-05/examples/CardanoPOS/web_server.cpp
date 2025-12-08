#include "web_server.h"
#include "transaction_display.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <stdlib.h> // For random number generation

namespace {
WebServer server(80);
bool serverStarted = false;

// Cardano address to receive payments (update this with your actual address)
const char *CARDANO_ADDRESS =
    "addr1qy..."; // TODO: Replace with your Cardano address
const char *TRANSACTIONS_FILE = "/transactions.json";

// Get MIME type based on file extension
String getContentType(String filename) {
  if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".json")) {
    return "application/json";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".svg")) {
    return "image/svg+xml";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".txt")) {
    return "text/plain";
  }
  return "application/octet-stream";
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

// Send JSON response with CORS headers
void sendJsonResponse(int code, const String &json) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(code, "application/json", json);
}

// Handle OPTIONS request for CORS preflight
void handleOptions() { sendJsonResponse(200, "{}"); }

// Handle POST request to create a new transaction
void handleCreateTransaction() {
  // Handle CORS preflight
  if (server.method() == HTTP_OPTIONS) {
    handleOptions();
    return;
  }

  // Get request body
  String body = "";
  if (server.hasArg("plain")) {
    body = server.arg("plain");
  } else {
    // Try to read from client directly
    while (server.client().available()) {
      body += (char)server.client().read();
    }
  }

  if (body.length() == 0) {
    sendJsonResponse(400, "{\"error\":\"Missing request body\"}");
    return;
  }

  // Parse JSON request
  StaticJsonDocument<256> requestDoc;
  DeserializationError error = deserializeJson(requestDoc, body);

  if (error) {
    sendJsonResponse(400, "{\"error\":\"Invalid JSON\"}");
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  if (!requestDoc.containsKey("amount") ||
      !requestDoc["amount"].is<unsigned long>()) {
    sendJsonResponse(400, "{\"error\":\"Missing or invalid amount\"}");
    return;
  }

  unsigned long lovelace = requestDoc["amount"].as<unsigned long>();

  // Get created_at from POST data (ISO string format)
  // Client sends created_at as ISO string (new Date().toISOString())
  String createdAt;
  if (requestDoc.containsKey("created_at") &&
      requestDoc["created_at"].is<const char *>()) {
    createdAt = requestDoc["created_at"].as<const char *>();
  }
  // If not provided, createdAt remains empty (will be stored as null)

  // Generate a random ID for the transaction
  // Use random number for uniqueness
  String transactionId = String(millis()) + "-" + String(random(10000, 99999));

  // Read existing transactions or create new array
  StaticJsonDocument<8192> doc;
  JsonArray transactions = doc.to<JsonArray>();

  if (LittleFS.exists(TRANSACTIONS_FILE)) {
    File file = LittleFS.open(TRANSACTIONS_FILE, "r");
    if (file) {
      String fileContent = file.readString();
      file.close();

      DeserializationError error = deserializeJson(doc, fileContent);
      if (error || !doc.is<JsonArray>()) {
        // If file is corrupted, start fresh
        doc.clear();
        transactions = doc.to<JsonArray>();
      } else {
        transactions = doc.as<JsonArray>();
      }
    }
  }

  // Create new transaction object
  JsonObject newTransaction = transactions.createNestedObject();
  newTransaction["id"] = transactionId;
  newTransaction["amount"] = lovelace;
  newTransaction["address"] = CARDANO_ADDRESS;
  if (createdAt.length() > 0) {
    newTransaction["created_at"] = createdAt; // ISO string format
  } else {
    newTransaction["created_at"] = nullptr; // null if not provided
  }
  newTransaction["txHash"] = "";

  // Write back to file
  File file = LittleFS.open(TRANSACTIONS_FILE, "w");
  if (!file) {
    sendJsonResponse(500, "{\"error\":\"Failed to open transactions file\"}");
    return;
  }

  serializeJson(doc, file);
  file.close();

  // Display transaction on TFT screen
  // Use current time for display (timestamp parameter is not used in display)
  unsigned long displayTimestamp = millis() / 1000;
  displayTransaction(lovelace, CARDANO_ADDRESS, displayTimestamp);

  // Return success response
  StaticJsonDocument<256> responseDoc;
  responseDoc["success"] = true;
  responseDoc["id"] = transactionId;

  String response;
  serializeJson(responseDoc, response);
  sendJsonResponse(200, response);

  Serial.print("Created transaction: ");
  Serial.print(lovelace);
  Serial.println(" lovelace");
}
} // namespace

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

  // Setup route handlers
  // API endpoint for creating transactions
  server.on("/api/transactions", HTTP_POST, handleCreateTransaction);
  server.on("/api/transactions", HTTP_OPTIONS, handleOptions);

  // Serve files from root and all subdirectories
  server.onNotFound(handleFileRequest);

  // Start the server
  server.begin();
  serverStarted = true;

  Serial.print("Web server started on http://");
  Serial.println(WiFi.localIP());
}

void webServerLoop() {
  // Handle incoming client requests
  if (serverStarted) {
    server.handleClient();
  }
}

bool webServerIsRunning() { return serverStarted; }
