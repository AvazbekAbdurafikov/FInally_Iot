# ESP32 OTA Update with WiFi and SPIFFS

This project demonstrates how to perform Over-The-Air (OTA) updates on an ESP32 device using WiFi and SPIFFS. The ESP32 connects to a WiFi network, periodically checks a server for updates, downloads the firmware if available, and performs the OTA update.

## Features

- Connects to a specified WiFi network.
- Checks for OTA updates from a server.
- Downloads firmware to SPIFFS.
- Performs OTA update from the downloaded firmware.
- Blinks an LED to indicate the device is running.

## Requirements

- ESP32 board
- Arduino IDE with ESP32 board support
- WiFi network
- Flask server to host the OTA updates

## Setup

1. **Install the necessary libraries:**
   Ensure you have the following libraries installed in the Arduino IDE:
   - WiFi.h
   - HTTPClient.h
   - SPIFFS.h
   - Update.h

2. **Configure WiFi credentials:**
   Modify the `ssid` and `password` variables in the code to match your WiFi network.

3. **Set up the Flask server:**
   - Create a Flask server to host the OTA updates.
   - Ensure the server provides endpoints for checking updates and downloading firmware. The server address should be configured in the `serverAddress` variable in the code.

4. **Upload the code to the ESP32:**
   - Connect your ESP32 to your computer.
   - Upload the provided code to the ESP32 using the Arduino IDE.

## Code Explanation

The main components of the code are:

- **WiFi Connection:**
  The `connectToWiFi` function connects the ESP32 to the specified WiFi network.

- **SPIFFS Initialization:**
  The SPIFFS is initialized in the `setup` function to store the downloaded firmware.

- **Checking for OTA Updates:**
  The `checkForOTAUpdate` function sends a request to the server to check if an update is available.

- **Downloading Firmware:**
  The `downloadFirmware` function downloads the firmware from the server and stores it in SPIFFS.

- **Performing OTA Update:**
  The `performOTAUpdate` function performs the OTA update using the firmware stored in SPIFFS.

- **LED Blinking:**
  The `blinkLED` function blinks an LED to indicate the device is running.

## Usage

1. **Power on the ESP32:**
   The ESP32 will connect to the WiFi network and start checking for updates.

2. **Monitor Serial Output:**
   Use the Arduino IDE Serial Monitor to view the output and debug messages.

3. **Server Interaction:**
   - The ESP32 will periodically check the server for updates.
   - If an update is available, it will download and perform the OTA update.

## Contributing

Feel free to open issues or submit pull requests if you have any improvements or bug fixes.

## License

This project is licensed under the MIT License.
