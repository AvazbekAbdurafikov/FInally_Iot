#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <Update.h>

// WiFi credentials
const char* ssid = "TURIN";
const char* password = "qwerty123";

// Server address
const char* serverAddress = "http://172.16.1.158:5000"; // Replace with your Flask server's IP address

#define LED_PIN 2 // Define the LED pin

void connectToWiFi();
void checkForOTAUpdate();
void downloadFirmware();
void clearSPIFFS();
void performOTAUpdate();
void checkFlags();
void blinkLED(void *parameter);

String macAddress;
unsigned long lastCheck = 0;
const unsigned long checkInterval = 20000; // 20 seconds
bool updateAvailable = false;
bool downloadFirmwareFlag = false;
bool otaUpdateFlag = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); // Initialize the LED pin as an output

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  connectToWiFi();
  macAddress = WiFi.macAddress();
  Serial.print("MAC Address: ");
  Serial.println(macAddress);

  // Create the LED blinking task
  xTaskCreate(
    blinkLED,     // Function that implements the task
    "Blink LED",  // Text name for the task
    1024,         // Stack size in words, not bytes
    NULL,         // Parameter passed into the task
    1,            // Priority at which the task is created
    NULL          // Used to pass back a handle by which the created task can be referenced
  );
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastCheck >= checkInterval) {
    lastCheck = currentMillis;
    checkForOTAUpdate();
    checkFlags(); // Check flags periodically
  }

  if (downloadFirmwareFlag) {
    clearSPIFFS(); // Clear SPIFFS before downloading new firmware
    downloadFirmware();
    downloadFirmwareFlag = false; // Reset the flag after downloading firmware
  }

  if (otaUpdateFlag) {
    performOTAUpdate();
    otaUpdateFlag = false; // Reset the flag after performing OTA update
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void clearSPIFFS() {
  Serial.println("Clearing SPIFFS...");
  SPIFFS.format();
  Serial.println("SPIFFS cleared");
}

void checkForOTAUpdate() {
  HTTPClient http;
  String url = String(serverAddress) + "/check_update";  // Corrected URL

  Serial.print("Checking for OTA update at: ");
  Serial.println(url);

  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Response: " + payload);
    if (payload.indexOf("\"update\":true") != -1) {
      Serial.println("Update available.");
      updateAvailable = true;
    }
  } else {
    Serial.print("Error checking for update: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void downloadFirmware() {
  HTTPClient http;
  String url = String(serverAddress) + "/download/firmware.bin";  // Corrected URL

  Serial.print("Downloading firmware from: ");
  Serial.println(url);

  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    File file = SPIFFS.open("/firmware.bin", FILE_WRITE);
    if (!file) {
      Serial.println("Failed to open file for writing");
      http.end();
      return;
    }

    int totalBytes = 0;
    uint8_t buffer[1024];
    int bytesRead = 0;
    while ((bytesRead = http.getStream().readBytes(buffer, sizeof(buffer))) > 0) {
      file.write(buffer, bytesRead);
      totalBytes += bytesRead;
      Serial.print(".");
    }

    file.close();
    Serial.printf("\nFirmware downloaded and stored in SPIFFS (%d bytes)\n", totalBytes);
  } else {
    Serial.print("Error downloading firmware: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void performOTAUpdate() {
  Serial.println("Starting OTA update...");
  File firmware = SPIFFS.open("/firmware.bin", "r");
  if (!firmware) {
    Serial.println("Failed to open firmware file");
    return;
  }

  if (!Update.begin(firmware.size())) {
    Serial.println("Not enough space to begin OTA");
    return;
  }

  size_t written = Update.writeStream(firmware);
  if (written == firmware.size()) {
    Serial.println("Written : " + String(written) + " successfully");
  } else {
    Serial.println("Written only : " + String(written) + "/" + String(firmware.size()) + ". Retry?");
    return;
  }

  if (Update.end()) {
    if (Update.isFinished()) {
      Serial.println("OTA update finished!");
      SPIFFS.remove("/firmware.bin"); // Remove firmware file after successful update
      ESP.restart();
    } else {
      Serial.println("OTA update not finished?");
    }
  } else {
    Serial.println("Error #: " + String(Update.getError()));
  }
}

void checkFlags() {
  HTTPClient http;
  String url = String(serverAddress) + "/check_flags?mac=" + macAddress;  // Corrected URL

  Serial.print("Checking flags at: ");
  Serial.println(url);

  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Flags Response: " + payload);
    if (payload.indexOf("\"download\":true") != -1) {
      Serial.println("Download flag is true.");
      downloadFirmwareFlag = true; // Set flag to download firmware
    }
    if (payload.indexOf("\"update\":true") != -1) {
      Serial.println("Update flag is true.");
      otaUpdateFlag = true; // Set flag to perform OTA update
    }
  } else {
    Serial.print("Error checking flags: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void blinkLED(void *parameter) {
  while (true) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}
