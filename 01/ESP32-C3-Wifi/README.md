# ESP32-C3 WiFi Connection Example

A simple Arduino sketch demonstrating how to connect an ESP32-C3 microcontroller to a WiFi network with proper error handling and timeout management.

## Features

- ✅ WiFi connection with credential management
- ✅ Connection timeout handling (20 seconds)
- ✅ Serial output for debugging and monitoring
- ✅ IP address display upon successful connection
- ✅ Secure credential storage using separate header file

## Hardware Requirements

- ESP32-C3 development board
- USB cable for programming and power
- Access to a 2.4GHz WiFi network

## Software Requirements

- Arduino IDE (1.8.x or 2.x)
- ESP32 Arduino Core installed

### Installing ESP32 Arduino Core

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search for "ESP32" and install "ESP32 by Espressif Systems"
6. Select your ESP32-C3 board from **Tools > Board > ESP32 Arduino**

## Setup Instructions

### 1. Clone or Download

```bash
git clone <repository-url>
cd ESP32-C3-Wifi
```

### 2. Configure WiFi Credentials

1. Copy `secrets-example.h` to `secrets.h`:
   ```bash
   cp secrets-example.h secrets.h
   ```

2. Edit `secrets.h` with your WiFi credentials:
   ```c
   #define WIFI_SSID "YourNetworkName"
   #define WIFI_PASSWORD "YourNetworkPassword"
   ```

3. **⚠️ Important**: Never commit `secrets.h` to version control. It's already included in `.gitignore`.

### 3. Upload to ESP32-C3

1. Connect your ESP32-C3 to your computer via USB
2. Open `ESP32-C3-Wifi.ino` in Arduino IDE
3. Select the correct board and port:
   - **Board**: ESP32C3 Dev Module (or your specific ESP32-C3 board)
   - **Port**: Select the appropriate COM/serial port
4. Click the **Upload** button

## Usage

1. After uploading, open the **Serial Monitor** (Tools > Serial Monitor)
2. Set the baud rate to **115200**
3. Press the **Reset** button on your ESP32-C3
4. Monitor the connection process:
   ```
   Connecting to WiFi...
   ......
   WiFi connected!
   IP address: 192.168.1.xxx
   ```

## Code Structure

### Main Components

- **`setup()`**: Initializes serial communication and establishes WiFi connection
- **`loop()`**: Main program loop (currently empty - ready for your additions)
- **`secrets.h`**: Contains WiFi credentials (not tracked in git)

### Key Features Explained

**Connection Timeout**: The code implements a 20-second timeout to prevent indefinite waiting:
```c
unsigned long startAttemptTime = millis();
const unsigned long timeout = 20000; // 20 seconds
```

**Status Monitoring**: Visual feedback during connection attempts:
```c
while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print("."); // Print dot while waiting
}
```

**Error Handling**: Clear feedback for both success and failure scenarios.

## Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| "Connecting to WiFi..." followed by timeout | Check WiFi credentials in `secrets.h` |
| Board not found/uploading fails | Ensure correct board and port selection |
| Compilation errors about missing secrets.h | Copy `secrets-example.h` to `secrets.h` |
| Can't connect to 5GHz network | ESP32-C3 only supports 2.4GHz WiFi |

### Debug Steps

1. **Verify credentials**: Double-check SSID and password in `secrets.h`
2. **Check network**: Ensure your WiFi network is 2.4GHz and broadcasting SSID
3. **Serial output**: Monitor Serial Monitor at 115200 baud for detailed logs
4. **Reset**: Try pressing the reset button after upload

## Next Steps

This basic WiFi connection example serves as a foundation for more advanced projects:

- 🌐 **Web Server**: Create a simple web server
- 📡 **HTTP Requests**: Make API calls to web services  
- 🔄 **OTA Updates**: Implement Over-The-Air firmware updates
- 📊 **IoT Integration**: Connect to cloud platforms (AWS IoT, Google Cloud, etc.)
- 🏠 **Home Automation**: Control devices over WiFi

## File Structure

```
ESP32-C3-Wifi/
├── ESP32-C3-Wifi.ino    # Main Arduino sketch
├── secrets-example.h     # Template for WiFi credentials
├── secrets.h            # Your WiFi credentials (gitignored)
└── README.md            # This documentation
```

## Security Notes

- ✅ WiFi credentials are stored in a separate header file
- ✅ `secrets.h` is excluded from version control
- ⚠️ Always use WPA2/WPA3 secured networks
- ⚠️ Never hardcode credentials in your main sketch

## License

This project is provided as-is for educational purposes. Feel free to modify and use in your own projects.

## Contributing

Found a bug or want to improve this example? Pull requests are welcome!

---

**Happy coding! 🚀**
