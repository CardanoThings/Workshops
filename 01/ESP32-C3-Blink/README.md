# ESP32-C3 Blink Example

A simple "Hello World" project for the ESP32-C3 microcontroller that blinks the onboard LED. This is perfect for beginners getting started with ESP32-C3 development and Arduino IDE.

## Overview

This project demonstrates the basic functionality of the ESP32-C3 by blinking its onboard LED at 1-second intervals. It's the equivalent of a "Hello World" program for embedded systems and serves as a foundation for learning ESP32-C3 programming.

## Hardware Requirements

- **ESP32-C3 Development Board** (any variant with onboard LED)
- **USB-C Cable** for programming and power
- **Computer** with Arduino IDE installed

## Features

- ✅ Simple LED blinking pattern (1 second on, 1 second off)
- ✅ Uses the onboard LED (GPIO8 on most ESP32-C3 boards)
- ✅ Well-documented code with clear comments
- ✅ Beginner-friendly Arduino sketch

## Code Explanation

### Pin Configuration

```cpp
#define LED_PIN 8 // Define the onboard LED pin for ESP32-C3 (GPIO8)
```

Most ESP32-C3 development boards have their onboard LED connected to GPIO8. This line defines a constant for easy reference throughout the code.

### Setup Function

```cpp
void setup() {
  pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
}
```

The `setup()` function runs once when the ESP32-C3 starts up. It configures GPIO8 as an output pin so we can control the LED.

### Main Loop

```cpp
void loop() {
  digitalWrite(LED_PIN, HIGH); // Turn the LED on
  delay(1000);                 // Wait for 1 second
  digitalWrite(LED_PIN, LOW);  // Turn the LED off
  delay(1000);                 // Wait for 1 second
}
```

The `loop()` function runs continuously after `setup()` completes. It:

1. Turns the LED ON by setting GPIO8 to HIGH (3.3V)
2. Waits for 1000 milliseconds (1 second)
3. Turns the LED OFF by setting GPIO8 to LOW (0V)
4. Waits another 1000 milliseconds
5. Repeats the cycle

## Getting Started

### Prerequisites

1. **Arduino IDE** (version 2.0 or later recommended)
2. **ESP32 Arduino Core** installed in Arduino IDE
3. **USB-C cable** compatible with your ESP32-C3 board

### Arduino IDE Setup

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add this URL to "Additional Board Manager URLs":

   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

4. Go to **Tools > Board > Boards Manager**
5. Search for "ESP32" and install "esp32 by Espressif Systems"
6. Select your ESP32-C3 board from **Tools > Board > ESP32 Arduino**

### Upload Instructions

1. Connect your ESP32-C3 to your computer via USB-C
2. Open `ESP32-C3-Blink.ino` in Arduino IDE
3. Select the correct board:
   - **Tools > Board > ESP32 Arduino > ESP32C3 Dev Module**
4. Select the correct port:
   - **Tools > Port > [Your ESP32-C3 Port]**
5. Click the **Upload** button (→) or press **Ctrl+U**

### Expected Behavior

After successful upload, you should see:

- The onboard LED blinking every second
- LED ON for 1 second → LED OFF for 1 second → repeat

## Troubleshooting

### Upload Issues

- **"Port not found"**: Make sure the USB cable supports data transfer (not just charging)
- **"Failed to connect"**: Hold the BOOT button while clicking upload, then release after upload starts
- **"Permission denied"**: On Linux/macOS, you might need to add your user to the dialout group

### LED Not Blinking

- **Wrong pin**: Some ESP32-C3 boards use different GPIO pins for the onboard LED (try GPIO2 or GPIO10)
- **Board variant**: Check your specific board's documentation for the correct LED pin

### Board Selection

Common ESP32-C3 board options in Arduino IDE:

- ESP32C3 Dev Module (generic)
- ESP32-C3-DevKitM-1
- ESP32-C3-DevKitC-02

## Next Steps

Once you have this basic example working, you can explore:

- **Serial Communication**: Add `Serial.begin(115200)` and `Serial.println()` for debugging
- **Button Input**: Read button presses to control the LED
- **WiFi Connectivity**: Connect to WiFi networks
- **Sensor Integration**: Read temperature, humidity, or other sensors
- **PWM Control**: Create breathing LED effects with `analogWrite()`

## File Structure

```text
ESP32-C3-Blink/
├── ESP32-C3-Blink.ino    # Main Arduino sketch
└── README.md             # This documentation
```

## Resources

- [ESP32-C3 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [Arduino ESP32 Core Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [ESP32-C3 Pinout Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitc-02.html)

## License

This project is open source and available under the [MIT License](LICENSE).

## Contributing

Feel free to submit issues and enhancement requests!

---

**Happy coding with ESP32-C3!** 🚀
