// Include necessary libraries
			#include <WiFi.h>
			#include <TFT_eSPI.h>
			#include <SPI.h>
			#include <qrcode.h>            // QR Code library for Arduino

			// Create TFT display object
			TFT_eSPI tft = TFT_eSPI();

			// WiFi credentials
			const char* ssid = "Your SSID";
			const char* password = "Your Password";

			// Payment details
			String recipientAddress = "addr_test1...";
			float paymentAmount = 0.0;
			String paymentURI = "";

			void setup() {
				Serial.begin(115200);
				
				// Initialize TFT display
				tft.init();
				tft.setRotation(1);        // Landscape orientation
				tft.fillScreen(TFT_BLACK);
				
				// Start WiFi connection
				WiFi.begin(ssid, password);
				while (WiFi.status() != WL_CONNECTED) {
					delay(1000);
					Serial.println("Connecting to WiFi...");
				}
				
				// Generate payment URI
				paymentAmount = 10.5;  // Example: 10.5 ADA
				paymentURI = createCIP13URI(paymentAmount, recipientAddress);
				
				// Generate and display QR code
				generateAndDisplayQRCode(paymentURI);
			}

			void loop() {
				// Your main loop - QR code stays displayed
				// You can add touch screen controls to regenerate or change amount
			}

			// Function to create CIP-13 payment URI
			String createCIP13URI(float amount, String address) {
				String uri = "web+cardano://";
				uri += address;
				
				if (amount > 0) {
					unsigned long lovelace = (unsigned long)(amount * 1000000);
					uri += "?amount=";
					uri += String(lovelace);
				}
				
				return uri;
			}

			// Function to generate and display QR code
			void generateAndDisplayQRCode(String data) {
				// Create QR code object
				// Parameters: version (1-40, higher = more data capacity), error correction level
				QRCode qrcode;
				uint8_t qrcodeData[qrcode_getBufferSize(3)];  // Version 3 QR code
				
				// Generate QR code
				// Parameters: buffer, version, error correction, data string
				qrcode_initText(&qrcode, qrcodeData, 3, ECC_MEDIUM, data.c_str());
				
				// Calculate QR code size and position for centering
				int qrSize = qrcode.size;
				int displaySize = min(200, 200);  // Maximum QR code size on display
				int scale = displaySize / qrSize;  // Scale factor
				int qrX = (240 - (qrSize * scale)) / 2;  // Center horizontally
				int qrY = (135 - (qrSize * scale)) / 2;   // Center vertically
				
				// Clear screen
				tft.fillScreen(TFT_BLACK);
				
				// Draw title
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Scan to Pay");
				
				// Draw QR code
				// Iterate through each module (pixel) of the QR code
				for (uint8_t y = 0; y < qrSize; y++) {
					for (uint8_t x = 0; x < qrSize; x++) {
						// Check if this module should be filled (black)
						if (qrcode_getModule(&qrcode, x, y)) {
							// Draw filled rectangle for each QR code module
							// Scale up the module size for visibility
							tft.fillRect(
								qrX + (x * scale),
								qrY + (y * scale),
								scale,
								scale,
								TFT_WHITE
							);
						} else {
							// Draw empty space (black/background)
							tft.fillRect(
								qrX + (x * scale),
								qrY + (y * scale),
								scale,
								scale,
								TFT_BLACK
							);
						}
					}
				}
				
				// Display payment amount below QR code
				tft.setTextSize(2);
				tft.setTextColor(TFT_GREEN, TFT_BLACK);
				tft.setCursor(10, 220);
				tft.print("Amount: ");
				tft.print(String(paymentAmount, 2));
				tft.println(" ADA");
			}