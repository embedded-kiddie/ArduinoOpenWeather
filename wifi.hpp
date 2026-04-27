//=====================================================================================
// ESP32 and UNO R4 WiFi compatible time management functions
//=====================================================================================
#pragma once

#if defined(ARDUINO_UNOR4_WIFI)

  #include <WiFiS3.h>

#else // ESP32

  #include <WiFi.h>

#endif

void wifiInit(void) {
  Serial.print("Connecting to WiFi network...");

#if defined(ARDUINO_UNOR4_WIFI)

  // Check for the WiFi module
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware.");
  }

  // Note: Checking the status first will connect more quickly
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(1000);
  }

#else // ESP32

  WiFi.mode(WIFI_STA); // not required (default: WIFI_STA)
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

//DBG_EXEC(WiFi.printDiag(Serial));

#endif // ARDUINO_UNOR4_WIFI or ESP32

  // print your board's IP address:
  Serial.println("done.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

  // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}