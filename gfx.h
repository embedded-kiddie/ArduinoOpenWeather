//=====================================================================================
// GUI for OpenWeather
//=====================================================================================
#pragma once

#include "config.h"
#include "OpenWeather.h"

// TFT screen size
#define TFT_WIDTH     (TFT_ROTATION & 1 ? 320 : 240)
#define TFT_HEIGHT    (TFT_ROTATION & 1 ? 240 : 320)

// Color definitions
#define TFT_BLACK     RGB565(  0,   0,   0) // 0x0000
#define TFT_BLUE      RGB565(  0,   0, 255) // 0x001F
#define TFT_RED       RGB565(255,   0,   0) // 0xF800
#define TFT_GREEN     RGB565(  0, 255,   0) // 0x07E0
#define TFT_CYAN      RGB565(  0, 255, 255) // 0x07FF
#define TFT_MAGENTA   RGB565(255,   0, 255) // 0xF81F
#define TFT_YELLOW    RGB565(255, 255,   0) // 0xFFE0
#define TFT_WHITE     RGB565(255, 255, 255) // 0xFFFF
#define TFT_ORANGE    RGB565(255, 180,   0) // 0xFDA0
#define TFT_LIGHTGREY RGB565(211, 211, 211) // 0xD69A

// Color icon
#define USE_COLOR_ICON  true

#if USE_COLOR_ICON
  typedef struct {
    uint16_t    color;
    const char  code;
    bool        next;
  } IconPack;
  static const IconPack *getWeatherIcon(uint16_t weather_id, bool day);
#else
  static const char *getWeatherIcon(uint16_t weather_id, bool day);
#endif

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
static int  findNextDay(WeatherData &data, int n);
static const char *getWeatherWind(uint8_t deg);
static const char *getWeatherDescription(uint16_t weather_id);
static void parseWeatherData(JsonDocument &doc, WeatherData &data);
static void printWeatherData(WeatherData &data);