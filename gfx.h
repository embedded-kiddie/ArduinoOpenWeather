//=====================================================================================
// GUI for OpenWeather
//=====================================================================================
#pragma once

#include "config.h"
#include "OpenWeather.h"

// Public
void gfxInit(void);
void gfxDrawSplashImage(void);
void gfxDrawCurrentTime(void);
void gfxDrawWeatherData(JsonDocument &doc);
void gfxDrawMessage(char const *msg, bool newline = true, int16_t X = 0);
int16_t gfxGetLastCursorX(void);

// Private
static void drawUpdateDateTime      (int X, int Y, int W, int H, WeatherData &data);
static void drawWeatherToday        (int X, int Y, int W, int H, WeatherData &data);
static void drawWeatherDescription  (int X, int Y, int W, int H, WeatherData &data);
static void drawTemperature         (int X, int Y, int W, int H, WeatherData &data);
static void drawWindIconSpeed       (int X, int Y, int W, int H, WeatherData &data);
static void drawWeatherForcast      (int X, int Y, int W, int H, WeatherData &data);
static void darwSunriseSunset       (int X, int Y, int W, int H, WeatherData &data);
static void drawMoonPhase           (int X, int Y, int W, int H, WeatherData &data);
static void drawWeatherCondition    (int X, int Y, int W, int H, WeatherData &data);
static void drawStringCenter(int16_t X, int16_t Y, int16_t W, int16_t H, const char *str);
static int findNextDay(WeatherData &data, int n);
static const char *getWeatherWind(uint8_t deg);
static const char *getWeatherDescription(uint16_t weather_id);
static void parseWeatherData(JsonDocument &doc, WeatherData &data);
static void printWeatherData(WeatherData &data);