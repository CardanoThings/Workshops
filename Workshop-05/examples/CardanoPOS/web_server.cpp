/**
 * web_server.cpp - HTTP web server implementation for Cardano POS system
 * 
 * This file implements an HTTP web server that serves static files from LittleFS
 * and provides REST API endpoints for managing Cardano payment transactions.
 * 
 * Features:
 * - Serves HTML, CSS, JavaScript, and image files from LittleFS
 * - REST API endpoint for creating payment transactions
 * - CORS support for cross-origin requests
 * - Transaction storage in JSON format on LittleFS
 * - Automatic display of transactions on TFT screen
 */

#include "web_server.h"
#include "transaction_display.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <stdlib.h> // For random number generation

namespace {
// HTTP server instance running on port 80 (standard HTTP port)
WebServer server(80);

// Track whether the server has been started successfully
bool serverStarted = false;

// Cardano address to receive payments (update this with your actual address)
// This address will be used for all payment requests
const char *CARDANO_ADDRESS =
    "addr1qy..."; // TODO: Replace with your Cardano address

// Path to the transactions JSON file stored in LittleFS
// This file stores all payment transaction records
const char *TRANSACTIONS_FILE = "/transactions.json";

/**
 * Get MIME content type based on file extension
 * 
 * Determines the appropriate HTTP Content-Type header based on the file
 * extension. This ensures browsers interpret files correctly (e.g., HTML
 * is rendered, CSS is applied, JavaScript is executed).
 * 
 * @param filename The filename or path to check for extension
 * @return The MIME type string (e.g., "text/html", "text/css")
 */
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
  // Default to binary data for unknown file types
  return "application/octet-stream";
}

/**
 * Handle HTTP file requests
 * 
 * Serves static files from LittleFS file system. Handles the following:
 * - Root path "/" redirects to "/index.html"
 * - Normalizes paths to start with "/"
 * - Streams files with appropriate MIME types
 * - Falls back to index.html for missing files (SPA routing support)
 * - Returns 404 for files that don't exist
 * 
 * This handler is registered for all unmatched routes (onNotFound),
 * allowing it to serve files from any path in LittleFS.
 */
void handleFileRequest() {
  String path = server.uri();

  // Default to index.html for root path
  // When user visits http://device-ip/, serve index.html
  if (path == "/" || path == "") {
    path = "/index.html";
  }

  // Ensure path starts with / (required for LittleFS)
  // LittleFS paths must be absolute, starting from root
  if (!path.startsWith("/")) {
    path = "/" + path;
  }

  // Check if file exists in LittleFS
  if (LittleFS.exists(path)) {
    // Determine MIME type based on file extension
    String contentType = getContentType(path);
    
    // Open file in read mode
    File file = LittleFS.open(path, "r");
    if (file) {
      // Stream file content to client with appropriate Content-Type header
      // This is memory-efficient for large files
      server.streamFile(file, contentType);
      file.close();
      Serial.print("Served file: ");
      Serial.println(path);
    } else {
      // File exists but couldn't be opened (permission issue or corruption)
      server.send(500, "text/plain", "Error opening file");
      Serial.print("Error opening file: ");
      Serial.println(path);
    }
  } else {
    // File not found - try index.html as fallback
    // This supports Single Page Application (SPA) routing where all
    // routes should serve index.html and let JavaScript handle routing
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
      // 404 Not Found - file doesn't exist and no fallback available
      server.send(404, "text/plain", "File not found");
      Serial.print("404 - File not found: ");
      Serial.println(path);
    }
  }
}

/**
 * Send JSON response with CORS headers
 * 
 * Sends an HTTP response with JSON content and Cross-Origin Resource Sharing
 * (CORS) headers. CORS headers allow web pages from other domains to make
 * requests to this API.
 * 
 * @param code HTTP status code (e.g., 200 for success, 400 for bad request)
 * @param json JSON string to send as response body
 */
void sendJsonResponse(int code, const String &json) {
  // Allow requests from any origin (for development)
  // In production, you might want to restrict this to specific domains
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(code, "application/json", json);
}

/**
 * Handle OPTIONS request for CORS preflight
 * 
 * Browsers send OPTIONS requests before making cross-origin POST requests
 * to check if the server allows the request. This handler responds with
 * appropriate CORS headers to allow the actual request.
 */
void handleOptions() { sendJsonResponse(200, "{}"); }

/**
 * Handle POST request to create a new payment transaction
 * 
 * API endpoint: POST /api/transactions
 * 
 * Creates a new Cardano payment transaction record. The transaction is stored
 * in a JSON file on LittleFS and displayed on the TFT screen.
 * 
 * Request body (JSON):
 *   {
 *     "amount": 1000000,           // Amount in lovelace (required)
 *     "created_at": "2024-01-01T12:00:00.000Z"  // ISO timestamp (optional)
 *   }
 * 
 * Response (JSON):
 *   {
 *     "success": true,
 *     "id": "12345-67890"  // Unique transaction ID
 *   }
 */
void handleCreateTransaction() {
  // Handle CORS preflight request
  // Browsers send OPTIONS before POST for cross-origin requests
  if (server.method() == HTTP_OPTIONS) {
    handleOptions();
    return;
  }

  // Get request body
  // The body contains JSON data with transaction details
  String body = "";
  if (server.hasArg("plain")) {
    // Standard way to get POST body
    body = server.arg("plain");
  } else {
    // Fallback: read directly from client stream
    // Some clients may not set the "plain" argument
    while (server.client().available()) {
      body += (char)server.client().read();
    }
  }

  // Validate that request body exists
  if (body.length() == 0) {
    sendJsonResponse(400, "{\"error\":\"Missing request body\"}");
    return;
  }

  // Parse JSON request
  // StaticJsonDocument<256> provides a fixed-size buffer for parsing
  // Increase size if you need to parse larger JSON objects
  StaticJsonDocument<256> requestDoc;
  DeserializationError error = deserializeJson(requestDoc, body);

  if (error) {
    sendJsonResponse(400, "{\"error\":\"Invalid JSON\"}");
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  // Validate required fields
  // Amount must be present and be an unsigned long (lovelace is always positive)
  if (!requestDoc.containsKey("amount") ||
      !requestDoc["amount"].is<unsigned long>()) {
    sendJsonResponse(400, "{\"error\":\"Missing or invalid amount\"}");
    return;
  }

  // Extract amount in lovelace (1 ADA = 1,000,000 lovelace)
  unsigned long lovelace = requestDoc["amount"].as<unsigned long>();

  // Get created_at timestamp from POST data (ISO string format)
  // Client typically sends this as: new Date().toISOString()
  // This allows tracking when the transaction was created on the client side
  String createdAt;
  if (requestDoc.containsKey("created_at") &&
      requestDoc["created_at"].is<const char *>()) {
    createdAt = requestDoc["created_at"].as<const char *>();
  }
  // If not provided, createdAt remains empty (will be stored as null)

  // Generate a unique transaction ID
  // Combines millis() (time since boot) with random number for uniqueness
  // Format: "millis-random" (e.g., "12345-67890")
  String transactionId = String(millis()) + "-" + String(random(10000, 99999));

  // Read existing transactions from file or create new array
  // StaticJsonDocument<8192> can hold up to 8192 bytes of JSON
  // Adjust size based on expected number of transactions
  StaticJsonDocument<8192> doc;
  JsonArray transactions = doc.to<JsonArray>();

  // Load existing transactions if file exists
  if (LittleFS.exists(TRANSACTIONS_FILE)) {
    File file = LittleFS.open(TRANSACTIONS_FILE, "r");
    if (file) {
      String fileContent = file.readString();
      file.close();

      // Parse existing transactions
      DeserializationError error = deserializeJson(doc, fileContent);
      if (error || !doc.is<JsonArray>()) {
        // If file is corrupted or invalid, start fresh
        // This prevents crashes from bad data
        doc.clear();
        transactions = doc.to<JsonArray>();
      } else {
        // Successfully loaded existing transactions
        transactions = doc.as<JsonArray>();
      }
    }
  }

  // Create new transaction object and add to array
  JsonObject newTransaction = transactions.createNestedObject();
  newTransaction["id"] = transactionId;
  newTransaction["amount"] = lovelace;
  newTransaction["address"] = CARDANO_ADDRESS; // Cardano address to receive payment
  if (createdAt.length() > 0) {
    newTransaction["created_at"] = createdAt; // ISO string format
  } else {
    newTransaction["created_at"] = nullptr; // null if not provided
  }
  newTransaction["txHash"] = ""; // Empty until transaction is confirmed on blockchain

  // Write updated transactions array back to file
  File file = LittleFS.open(TRANSACTIONS_FILE, "w");
  if (!file) {
    sendJsonResponse(500, "{\"error\":\"Failed to open transactions file\"}");
    return;
  }

  // Serialize JSON document to file
  serializeJson(doc, file);
  file.close();

  // Display transaction on TFT screen
  // This provides immediate visual feedback to the user
  // Use current time for display (timestamp parameter is not used in display)
  unsigned long displayTimestamp = millis() / 1000;
  displayTransaction(lovelace, CARDANO_ADDRESS, displayTimestamp);

  // Return success response with transaction ID
  StaticJsonDocument<256> responseDoc;
  responseDoc["success"] = true;
  responseDoc["id"] = transactionId;

  String response;
  serializeJson(responseDoc, response);
  sendJsonResponse(200, response);

  // Log transaction creation for debugging
  Serial.print("Created transaction: ");
  Serial.print(lovelace);
  Serial.println(" lovelace");
}
} // namespace

/**
 * Initialize the web server
 * 
 * Sets up the HTTP web server with the following steps:
 * 1. Mounts LittleFS file system for serving static files
 * 2. Lists all available files (for debugging)
 * 3. Registers API route handlers for transaction management
 * 4. Sets up file serving for all unmatched routes
 * 5. Starts the HTTP server on port 80
 * 
 * This function should be called after WiFi is connected. If LittleFS
 * fails to mount, the server will not start.
 */
void webServerSetup() {
  // Initialize LittleFS file system
  // The 'true' parameter formats the filesystem if it doesn't exist
  // This is useful for first-time setup, but be careful in production
  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: LittleFS Mount Failed");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  // List all files in LittleFS (for debugging)
  // This helps verify that files were uploaded correctly
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
  // Register API endpoints before file serving so they take precedence
  
  // API endpoint for creating transactions (POST request)
  server.on("/api/transactions", HTTP_POST, handleCreateTransaction);
  
  // CORS preflight handler for the transactions endpoint
  server.on("/api/transactions", HTTP_OPTIONS, handleOptions);

  // Serve files from root and all subdirectories
  // onNotFound catches all routes that don't match registered handlers
  // This allows serving files from any path in LittleFS
  server.onNotFound(handleFileRequest);

  // Start the HTTP server
  // Server begins listening on port 80 for incoming connections
  server.begin();
  serverStarted = true;

  // Print server URL for easy access
  Serial.print("Web server started on http://");
  Serial.println(WiFi.localIP());
}

/**
 * Handle incoming server requests
 * 
 * Processes HTTP requests from clients. This function must be called
 * repeatedly in the main loop() to handle incoming connections.
 * 
 * The server handles requests asynchronously, so this should be called
 * frequently (every loop iteration) to maintain responsiveness and
 * prevent connection timeouts.
 */
void webServerLoop() {
  // Handle incoming client requests
  // Only process requests if server has been successfully started
  if (serverStarted) {
    server.handleClient();
  }
}

/**
 * Check if the web server is currently running
 * 
 * @return true if the server has been started and is running, false otherwise
 */
bool webServerIsRunning() { return serverStarted; }
