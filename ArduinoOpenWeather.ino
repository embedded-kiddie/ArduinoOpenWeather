//=====================================================================================
// OpenWeather forcast application for UNO R4 WiFi and ESP32
//  Auther: embedded-kiddie (https://github.com/embedded-kiddie)
//  Released under the MIT license (https://opensource.org/license/mit)
//
// Linked libraries (tested):
//  UNO R4 WiFi (1.6.0):
//    SPI
//    Wire
//    WiFiS3 at version 0.0.0 (Firmware version: 0.6.0)
//    RTC at version 1.0
//    NTPClient at version 3.2.1
//    ArduinoJson at version 7.4.3
//    GFX Library for Arduino at version 1.6.5
//  ESP32 (3.3.9):
//    WiFi at version 3.3.9
//    Networking at version 3.3.9
//    NetworkClientSecure at version 3.3.9
//    SPI at version 3.3.9
//    Wire at version 3.3.9
//    ArduinoJson at version 7.4.3
//    GFX Library for Arduino at version 1.6.5
//=====================================================================================
#include "config.h"
#include "rtcntp.h"
#include "OpenWeather.h"
#include "gfx.h"
#include "wifi.hpp"

// Macros that perform tasks periodically
#define DO_EVERY(period, lastTime)  static uint32_t lastTime = 0; for (uint32_t now = millis(); now - lastTime >= period; lastTime = now)

static OpenWeather weather;

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 1000);

  gfxInit();
  wifiInit();
  while (!rtcInit()) {
    delay(1000);
  }

  // Get the first weather data
  weather.Init();
  while (!weather.RequestWeatherData()) {
    for (int i = 10; i > 0; i--) {
      char buf[32];
      snprintf(buf, sizeof(buf), "Waiting for a retry: %2d", i);
      gfxDrawMessage(buf);
      delay(1000);
    }
  }

  gfxDrawWeatherData(weather.data);
}

void loop() {
  DO_EVERY(NTP_SYNC_INTERVAL, lastTime1) {
    rtcSyncNTP();
  }

  gfxDrawCurrentTime();

  DO_EVERY(HTTP_REQUEST_INTERVAL, lastTime2) {
    // Get weather data and draw graphics
    if (weather.RequestWeatherData()) {
      gfxDrawWeatherData(weather.data);
    }
  }
}