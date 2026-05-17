//=====================================================================================
// UNO R4 WiFi and ESP32 compatible WiFi management function
//=====================================================================================
#pragma once

#if defined(ARDUINO_UNOR4_WIFI)

  #include <WiFiS3.h>

#else // ESP32

  #include <WiFi.h>

#endif

void wifiInit(void) {
  gfxDrawMessage("\nConnecting to WiFi...");

#if defined(ARDUINO_UNOR4_WIFI)

  // Check for the WiFi module
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    DBG_EXEC(Serial.println("Please upgrade the firmware."));
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
  gfxDrawMessage("done.\n", false);
  DBG_EXEC(Serial.print("IP address: "));
  DBG_EXEC(Serial.println(WiFi.localIP().toString()));

  // print the received signal strength:
  DBG_EXEC(Serial.print("signal strength (RSSI):"));
  DBG_EXEC(Serial.print(WiFi.RSSI()));
  DBG_EXEC(Serial.println(" dBm"));
}