//=====================================================================================
// OpenWeather forcast application for UNO R4 WiFi and ESP32
//=====================================================================================
#include "config.h"
#include "rtcntp.h"
#include "OpenWeather.h"
#include "gfx.h"
#include "wifi.hpp"

uint32_t updateTime = 0;
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
    delay(10000);
  }

  updateTime = millis();
  gfxDrawWeatherData(weather.data);
}

void loop() {
  rtcSyncNTP();
  gfxDrawCurrentTime();

  if (millis() - updateTime > HTTP_REQUEST_INTERVAL) {
    updateTime = millis();

    // Get weather data and draw graphics
    if (weather.RequestWeatherData()) {
      gfxDrawWeatherData(weather.data);
    }
  }
}