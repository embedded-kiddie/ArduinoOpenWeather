//=====================================================================================
// OpenWeather forcast application for UNO R4 WiFi and ESP32
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
  DBG_EXEC({
    Serial.begin(115200);
    while (!Serial || millis() < 1000);
  });

  gfxInit();
  wifiInit();
  rtcInit();

  // Get the first weather data
  weather.Init();
  while (!weather.RequestWeatherData()) {
    // Retry in 10 seconds
    delay(3000);
    gfxDrawMessage("Waiting for a retry...\n");
    delay(7000);
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