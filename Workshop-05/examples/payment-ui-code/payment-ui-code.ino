// Include necessary libraries
			#include <TFT_eSPI.h>          // TFT display library
			#include <SPI.h>               // SPI communication

			// Create TFT display object
			TFT_eSPI tft = TFT_eSPI();

			// Payment interface state
			enum PaymentState {
				STATE_AMOUNT_INPUT,    // User is entering payment amount
				STATE_CONFIRM,         // Showing confirmation screen
				STATE_QR_DISPLAY       // Displaying QR code
			};

			PaymentState currentState = STATE_AMOUNT_INPUT;

			// Payment amount input
			String amountInput = "";
			float paymentAmount = 0.0;
			String recipientAddress = "addr_test1...";  // Your wallet address to receive payments

			// Button coordinates and dimensions
			struct Button {
				int x, y, width, height;
				String label;
			};

			// Number pad buttons (0-9, decimal point, backspace, clear)
			Button numPad[12] = {
				{20, 80, 40, 40, "1"},   {70, 80, 40, 40, "2"},   {120, 80, 40, 40, "3"},
				{20, 130, 40, 40, "4"},  {70, 130, 40, 40, "5"},  {120, 130, 40, 40, "6"},
				{20, 180, 40, 40, "7"},  {70, 180, 40, 40, "8"},  {120, 180, 40, 40, "9"},
				{70, 230, 40, 40, "0"},  {20, 230, 40, 40, "."},  {120, 230, 40, 40, "C"}
			};

			// Action buttons
			Button confirmButton = {170, 80, 60, 100, "OK"};
			Button cancelButton = {170, 190, 60, 80, "Cancel"};

			void setup() {
				// Initialize serial communication for debugging
				Serial.begin(115200);
				
				// Initialize TFT display
				tft.init();
				tft.setRotation(1);        // Landscape orientation
				tft.fillScreen(TFT_BLACK);
				
				// Draw initial payment interface
				drawPaymentInterface();
			}

			void loop() {
				// Check for touch input
				uint16_t x, y;
				bool pressed = tft.getTouch(&x, &y);
				
				if (pressed) {
					// Handle touch based on current state
					handleTouch(x, y);
					delay(200);  // Debounce delay
				}
			}

			void drawPaymentInterface() {
				// Clear screen
				tft.fillScreen(TFT_BLACK);
				
				// Draw title
				tft.setTextSize(2);
				tft.setTextColor(TFT_CYAN, TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Payment Amount");
				
				// Draw amount display area (with border)
				tft.fillRect(10, 40, 220, 30, TFT_DARKGREY);
				tft.drawRect(10, 40, 220, 30, TFT_WHITE);
				
				// Display current amount input
				tft.setTextSize(2);
				tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
				tft.setCursor(15, 45);
				if (amountInput.length() > 0) {
					tft.print(amountInput);
					tft.print(" ADA");
				} else {
					tft.print("0.00 ADA");
				}
				
				// Draw number pad buttons
				drawNumberPad();
				
				// Draw action buttons
				drawActionButtons();
			}

			void drawNumberPad() {
				// Draw all number pad buttons
				for (int i = 0; i < 12; i++) {
					drawButton(numPad[i]);
				}
			}

			void drawActionButtons() {
				// Draw confirm button
				drawButton(confirmButton);
				
				// Draw cancel button
				drawButton(cancelButton);
			}

			void drawButton(Button btn) {
				// Draw button background
				tft.fillRect(btn.x, btn.y, btn.width, btn.height, TFT_BLUE);
				
				// Draw button border
				tft.drawRect(btn.x, btn.y, btn.width, btn.height, TFT_WHITE);
				
				// Draw button label (centered)
				tft.setTextSize(2);
				tft.setTextColor(TFT_WHITE, TFT_BLUE);
				
				// Calculate text position to center it
				int textX = btn.x + (btn.width / 2) - (btn.label.length() * 6);
				int textY = btn.y + (btn.height / 2) - 8;
				
				tft.setCursor(textX, textY);
				tft.println(btn.label);
			}

			void handleTouch(uint16_t x, uint16_t y) {
				// Check if touch is within number pad buttons
				for (int i = 0; i < 12; i++) {
					if (isPointInButton(x, y, numPad[i])) {
						handleNumberPadInput(numPad[i].label);
						return;
					}
				}
				
				// Check confirm button
				if (isPointInButton(x, y, confirmButton)) {
					handleConfirm();
					return;
				}
				
				// Check cancel button
				if (isPointInButton(x, y, cancelButton)) {
					handleCancel();
					return;
				}
			}

			bool isPointInButton(uint16_t x, uint16_t y, Button btn) {
				// Check if point (x, y) is within button boundaries
				return (x >= btn.x && x <= btn.x + btn.width &&
				        y >= btn.y && y <= btn.y + btn.height);
			}

			void handleNumberPadInput(String input) {
				if (input == "C") {
					// Clear input
					amountInput = "";
				} else if (input == ".") {
					// Add decimal point if not already present
					if (amountInput.indexOf(".") == -1) {
						amountInput += ".";
					}
				} else {
					// Add digit to input
					amountInput += input;
				}
				
				// Update display
				drawPaymentInterface();
			}

			void handleConfirm() {
				// Convert input string to float
				if (amountInput.length() > 0) {
					paymentAmount = amountInput.toFloat();
					
					// Validate amount (must be positive)
					if (paymentAmount > 0) {
						// Move to confirmation state
						currentState = STATE_CONFIRM;
						drawConfirmationScreen();
					}
				}
			}

			void handleCancel() {
				// Reset to initial state
				amountInput = "";
				paymentAmount = 0.0;
				currentState = STATE_AMOUNT_INPUT;
				drawPaymentInterface();
			}

			void drawConfirmationScreen() {
				// Clear screen
				tft.fillScreen(TFT_BLACK);
				
				// Draw title
				tft.setTextSize(2);
				tft.setTextColor(TFT_YELLOW, TFT_BLACK);
				tft.setCursor(10, 10);
				tft.println("Confirm Payment");
				
				// Display payment amount
				tft.setTextSize(3);
				tft.setTextColor(TFT_GREEN, TFT_BLACK);
				tft.setCursor(10, 50);
				tft.print(String(paymentAmount, 2));
				tft.setTextSize(2);
				tft.println(" ADA");
				
				// Display recipient address (truncated)
				tft.setTextSize(1);
				tft.setTextColor(TFT_GRAY, TFT_BLACK);
				tft.setCursor(10, 100);
				tft.print("To: ");
				tft.println(recipientAddress.substring(0, 20) + "...");
				
				// Draw confirm and cancel buttons
				Button finalConfirm = {20, 150, 100, 40, "Confirm"};
				Button finalCancel = {120, 150, 100, 40, "Cancel"};
				
				drawButton(finalConfirm);
				drawButton(finalCancel);
			}