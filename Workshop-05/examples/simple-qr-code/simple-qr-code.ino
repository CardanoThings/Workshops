// Simple QR code generation example
			#include <TFT_eSPI.h>
			#include <qrcode.h>

			TFT_eSPI tft = TFT_eSPI();

			void setup() {
				tft.init();
				tft.setRotation(1);
				tft.fillScreen(TFT_BLACK);
				
				// Text to encode in QR code
				String text = "Hello Cardano!";
				
				// Generate QR code
				QRCode qrcode;
				uint8_t qrcodeData[qrcode_getBufferSize(3)];
				qrcode_initText(&qrcode, qrcodeData, 3, ECC_MEDIUM, text.c_str());
				
				// Display QR code
				int scale = 3;  // Scale factor
				int offsetX = 50;
				int offsetY = 20;
				
				for (uint8_t y = 0; y < qrcode.size; y++) {
					for (uint8_t x = 0; x < qrcode.size; x++) {
						if (qrcode_getModule(&qrcode, x, y)) {
							tft.fillRect(
								offsetX + (x * scale),
								offsetY + (y * scale),
								scale,
								scale,
								TFT_WHITE
							);
						}
					}
				}
			}

			void loop() {
				// Your main loop
			}