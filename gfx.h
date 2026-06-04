//=============================================================================
// GUI for OpenWeather
//=============================================================================
#pragma once

#include "config.h"
#include "OpenWeather.h"

// TFT screen size
#define TFT_WIDTH     (TFT_ROTATION & 1 ? 320 : 240)
#define TFT_HEIGHT    (TFT_ROTATION & 1 ? 240 : 320)

// Color definitions
#define TFT_BLACK     0x0000  //   0,   0,   0
#define TFT_BLUE      0x001F  //   0,   0, 255
#define TFT_RED       0xF800  // 255,   0,   0
#define TFT_GREEN     0x07E0  //   0, 255,   0
#define TFT_CYAN      0x07FF  //   0, 255, 255
#define TFT_MAGENTA   0xF81F  // 255,   0, 255
#define TFT_YELLOW    0xFFE0  // 255, 255,   0
#define TFT_WHITE     0xFFFF  // 255, 255, 255
#define TFT_ORANGE    0xFDA0  // 255, 180,   0
#define TFT_LIGHTGREY 0xD69A  // 211, 211, 211

// Widget color
#define COLOR_DATE    TFT_WHITE
#define COLOR_ICON    TFT_WHITE
#define COLOR_WIND    TFT_GREEN
#define COLOR_TIME    TFT_YELLOW
#define COLOR_TEMP    TFT_CYAN
#define COLOR_TITLE   TFT_ORANGE
#define COLOR_VALUE   TFT_WHITE

// Splash message
#define SPLASH_MSG_X  8
#define SPLASH_MSG_Y  (TFT_HEIGHT * 3 / 4)

// Public
void gfxInit(void);
void gfxDrawLogo(void);
void gfxDrawCurrentTime(void);
void gfxDrawWeatherData(JsonDocument &doc);
void gfxDrawMessage(char const *msg, bool newline = true);

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
static const char *getWeatherIcon(uint16_t weather_id, bool day);
static const char *getWeatherDescription(uint16_t weather_id);
static void parseWeatherData(JsonDocument &doc, WeatherData &data);
static void printWeatherData(WeatherData &data);