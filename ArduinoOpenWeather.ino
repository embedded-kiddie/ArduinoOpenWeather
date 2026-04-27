//=====================================================================================
// OpenWeather monitoring for ESP32
//=====================================================================================
#include "config.h"
#include "wifi.hpp"
#include "rtcntp.h"
#include "gfx.h"
#include "OpenWeather.h"

static OpenWeather weather;
static uint32_t updateTime;

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 1000);

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
  gfxDrawCurrentTime();

  if (millis() - updateTime > UPDATE_INTERVAL) {
    updateTime = millis();

    // Get weather data and draw graphics
    if (weather.RequestWeatherData()) {
      gfxDrawWeatherData(weather.data);
    }
  }
}