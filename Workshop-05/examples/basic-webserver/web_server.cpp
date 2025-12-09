#include "web_server.h"
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>

namespace {
WebServer server(80);       // Web server on port 80
bool serverStarted = false; // Flag to check if server is started

// Get MIME type for HTML files
String getContentType(String filename) { return "text/html"; }

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

  // Serve files from root and all subdirectories
  server.onNotFound(handleFileRequest);

  // Start the server
  server.begin();
  serverStarted = true;

  Serial.print("Web server started on http://");
  Serial.println(WiFi.localIP());
}

// Handle incoming client requests
void webServerLoop() {
  // Handle incoming client requests
  if (serverStarted) {
    server.handleClient();
  }
}

// Check if server is running
bool webServerIsRunning() { return serverStarted; }
