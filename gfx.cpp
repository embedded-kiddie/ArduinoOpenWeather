//=============================================================================
// GUI for OpenWeatherMap
//=============================================================================
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "rtcntp.h"
#include "OpenWeather.h"
#include "gfx.h"
#include "logo.hpp"
#include "moon.hpp"

//---------------------------------------------------------------------------------------------
// GFX Library for Arduino
// https://github.com/moononournation/Arduino_GFX
// https://github.com/moononournation/ArduinoFreeFontFile
//---------------------------------------------------------------------------------------------
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "fonts/NotoSans/SemiBoldItalicNumeric16pt7b.h" // bitmap font
#include "fonts/NotoSans/SemiBoldAlphabet7pt7b.h"       // bitmap font
#include "fonts/Icons/WeatherIcons_Symbols_min48pt7b.h" // bitmap font
#include "fonts/Icons/WeatherIcons_Symbols_min35pt7b.h" // bitmap font
#include "fonts/Icons/WeatherIcons_Arrows30pt7b.h"      // bitmap font

#define ICON_SMALL_HEIGHT 45
#define ICON_LARGE_HEIGHT 60
#define FONT_LARGE_HEIGHT 22
#define FONT_SMALL_HEIGHT 12
#define FONT_SMALL_LINEFD 6

#if   true
#define GFX(f)  tft->f
#else
#define GFX(f)
#endif

#if   false
#define GFX_DBG(f)  GFX(f)
#else
#define GFX_DBG(f)
#endif

#define SPI_FREQ    40000000  // or 80000000

#if defined(ARDUINO_UNOR4_WIFI)
  Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
  Arduino_GFX *tft = new Arduino_ILI9341(bus, TFT_RST);
#else // ESP32
  Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);
  Arduino_GFX *tft = new Arduino_ILI9341(bus, TFT_RST);
#endif

//---------------------------------------------------------------------------------------------
// Initialize Screen and Draw Splash Image
//---------------------------------------------------------------------------------------------
void gfxInit(void) {
  // Turn on the backlight
  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }

  // Initialize LCD
  GFX(begin(SPI_FREQ));
  GFX(setRotation(TFT_ROTATION));
  GFX(fillScreen(TFT_WHITE));

  gfxDrawLogo();
}

//---------------------------------------------------------------------------------------------
// https://openweather.co.uk/brand_guidelines
//---------------------------------------------------------------------------------------------
void gfxDrawLogo(void) {
  GFX(drawIndexedBitmap(
    (GFX(width ()) - OPENWEATHERLOGO_WIDTH ) / 2,   // X
    (GFX(height()) - OPENWEATHERLOGO_HEIGHT) / 3,   // Y
    const_cast<uint8_t  *>(OpenWeatherLogoIndexed), // Indexed bitmap
    const_cast<uint16_t *>(OpenWeatherLogoPalette), // Color Index
    OPENWEATHERLOGO_WIDTH, OPENWEATHERLOGO_HEIGHT)  // Width, Height
  );
}

//---------------------------------------------------------------------------------------------
// Draw Current Time
//---------------------------------------------------------------------------------------------
void gfxDrawCurrentTime(void) {
  static time32_t time;
  time32_t now;
  rtcCurrentTime(&now);

  if (time != now) {
    time = now;
#if (TFT_ROTATION == 0) || (TFT_ROTATION == 2)  // Portrait
    GFX(setCursor(60, 22 + FONT_LARGE_HEIGHT));
#else
    GFX(setCursor(30, 22 + FONT_LARGE_HEIGHT));
#endif
    GFX(setTextColor(TFT_YELLOW, TFT_BLACK));
    GFX(setFont(&NotoSans_SemiBoldItalicNumeric16pt7b));
    GFX(print(rtcTimeString(now)));
  }
}

//---------------------------------------------------------------------------------------------
// Draw Weather Data on the Screen
//---------------------------------------------------------------------------------------------
void gfxDrawWeatherData(JsonDocument &doc) {
  // Parse JSON to Weather data
  WeatherData data;
  parseWeatherData(doc, data);
  doc.clear();

#if defined(ARDUINO_UNOR4_WIFI)
//rtcUpdate(data.timezone);
#endif

  int16_t X, Y, W, H;

  GFX(fillScreen(TFT_BLACK));

#if (TFT_ROTATION == 0) || (TFT_ROTATION == 2)  // Portrait
  drawUpdateDateTime      (X =  25, Y =   0, W = 190, H = 14, data);  // Update date and time
  drawWeatherToday        (X =   0, Y =  56, W =  80, H = 88, data);  // Today's weather icon
  drawWeatherDescription  (X = 100, Y =  56, W = 140, H = 14, data);  // Today's weather description
  drawTemperature         (X =  80, Y =  72, W =  80, H = 72, data);  // Today's temperature
  drawWindIconSpeed       (X = 160, Y =  72, W =  80, H = 72, data);  // Wind icon and speed
  drawWeatherForcast      (X =   0, Y = 158, W =  60, H = 82, data);  // 4 days weather forecast
  darwSunriseSunset       (X =   0, Y = 250, W =  80, H = 70, data);  // Sunrise, Sunset
  drawMoonPhase           (X =  80, Y = 250, W =  80, H = 70, data);  // Moon phase
  drawWeatherCondition    (X = 160, Y = 250, W =  80, H = 70, data);  // Cloudiness, Air pressure
#else // Landscape
  drawUpdateDateTime      (X =   0, Y =   0, W = 190, H = 14, data);  // Update date and time
  drawWeatherToday        (X =   0, Y =  58, W =  80, H = 88, data);  // Today's weather icon
  drawWeatherDescription  (X = 100, Y =  58, W = 220, H = 14, data);  // Today's weather description
  drawTemperature         (X =  85, Y =  72, W =  80, H = 70, data);  // Today's temperature
  drawWindIconSpeed       (X = 170, Y =  72, W =  80, H = 70, data);  // Wind icon and speed
  drawWeatherCondition    (X = 250, Y =  72, W =  70, H = 70, data);  // Cloudiness, Air pressure
  darwSunriseSunset       (X = 180, Y =  20, W = 140, H = 30, data);  // Sunrise, Sunset
  drawWeatherForcast      (X =   0, Y = 158, W =  60, H = 82, data);  // 4 days weather forecast
  drawMoonPhase           (X = 240, Y = 160, W =  80, H = 76, data);  // Moon phase
#endif
}

//---------------------------------------------------------------------------------------------
// Update Date and Time
//---------------------------------------------------------------------------------------------
static void drawUpdateDateTime(int X, int Y, int W, int H, WeatherData &data) {
  char buf[32];
  snprintf(buf, sizeof(buf), "Updated: %s", rtcDateString(data.time).c_str());
  GFX(setTextColor(COLOR_DATE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y - 4, W, H, buf);

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Today's Day of Week and Weather Icon
//---------------------------------------------------------------------------------------------
static void drawWeatherToday(int X, int Y, int W, int H, WeatherData &data) {
  GFX(setTextColor(COLOR_TITLE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y, W, H, rtcWeekString(data.weather[0].time).c_str());

  struct tm tm;
  rtcConvertTime(data.time, &tm);
  uint32_t t = tm.tm_hour * 60 + tm.tm_min;
  char const *icon = getWeatherIcon(data.weather[0].id, (data.sunrise <= t && t <= data.sunset));
  GFX(setTextColor(COLOR_ICON));
  GFX(setFont(&WeatherIcons_Symbols_min48pt7b));
  drawStringCenter(X, Y + 78, W, 0, icon);

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Today's Weather Description
//---------------------------------------------------------------------------------------------
static void drawWeatherDescription(int X, int Y, int W, int H, WeatherData &data) {
  GFX(setTextColor(COLOR_VALUE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y, 0, H, getWeatherDescription(data.weather[0].id));

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Today's Temperature 
//---------------------------------------------------------------------------------------------
static void drawTemperature(int X, int Y, int W, int H, WeatherData &data) {
  char buf[16];
  snprintf(buf, sizeof(buf), "% 5.1f", SCALE_MUL(float, data.weather[0].temp, 0.1f));
  GFX(setTextColor(COLOR_TEMP));
  GFX(setFont(&NotoSans_SemiBoldItalicNumeric16pt7b));
  drawStringCenter(X, Y + 24, W, H, buf);

  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  GFX(setCursor(X + 64, Y + 18));
  GFX(print("!C"));

  GFX(setTextColor(COLOR_VALUE));
  snprintf(buf, sizeof(buf), "%d %%RH", (int)data.weather[0].humidity);
  drawStringCenter(X, Y + H - FONT_SMALL_HEIGHT, W, H, buf);

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Wind Icon and Wind Speed 
//---------------------------------------------------------------------------------------------
static void drawWindIconSpeed(int X, int Y, int W, int H, WeatherData &data) {
  char const *icon = getWeatherWind(data.weather[0].wind_deg);
  GFX(setTextColor(COLOR_WIND));
  GFX(setFont(&WeatherIcons_Arrows30pt7b));
  drawStringCenter(X, Y + 6, W, H, icon);

  char buf[16];
  snprintf(buf, sizeof(buf), "%4.1f m/s", SCALE_MUL(float, data.weather[0].wind_speed, 0.1f));
  GFX(setTextColor(COLOR_VALUE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y + H - FONT_SMALL_HEIGHT, W, H, buf);

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Weather Forecast for 4 days 
//---------------------------------------------------------------------------------------------
static void drawWeatherForcast(int X, int Y, int W, int H, WeatherData &data) {
  char buf[16];
  const int n = data.n_weather;

  for (int i = findNextDay(data, 0), N = 0; i < n && N < 4; i += 8, N++, X += W) {
    GFX(setTextColor(COLOR_TITLE));
    GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
    drawStringCenter(X, Y, W, H, rtcWeekString(data.weather[i + 4].time).c_str());

    int tmin = 999, tmax = -999;
    for (int j = i; j < i + 8; j++) {
      if (tmin > data.weather[j].temp) { tmin = data.weather[j].temp; }
      if (tmax < data.weather[j].temp) { tmax = data.weather[j].temp; }
    }

    snprintf(buf, sizeof(buf), "%d!/%d!", SCALE_DIV(int, tmin, 10), SCALE_DIV(int, tmax, 10));
    GFX(setTextColor(COLOR_VALUE));
    drawStringCenter(X, Y + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD), W, H, buf);

    // Display the weather icon at intermediate time (i + 4).
    struct tm tm;
    rtcConvertTime(data.weather[i + 4].time, &tm);
    uint32_t t = tm.tm_hour * 60 + tm.tm_min;

    const char *icon = getWeatherIcon(data.weather[i + 4].id, (data.sunrise <= t && t <= data.sunset));
    GFX(setTextColor(COLOR_ICON));
    GFX(setFont(&WeatherIcons_Symbols_min35pt7b));
    drawStringCenter(X - 5, Y + 78, W, 0, icon);

    GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
  }
}

//---------------------------------------------------------------------------------------------
// Sunrise, Sunset
//---------------------------------------------------------------------------------------------
static void darwSunriseSunset(int X, int Y, int W, int H, WeatherData &data) {
#if (TFT_ROTATION == 0) || (TFT_ROTATION == 2)  // Portrait
  char buf[16];
  snprintf(buf, sizeof(buf), "Sun");
  GFX(setTextColor(COLOR_TITLE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y + 8, W, H, buf);

  GFX(setTextColor(COLOR_VALUE));
  snprintf(buf, sizeof(buf), "%02d:%02d", (int)(data.sunrise / 60), (int)(data.sunrise % 60));
  drawStringCenter(X, Y + 8 + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD), W, H, buf);

  snprintf(buf, sizeof(buf), "%02d:%02d", (int)(data.sunset  / 60), (int)(data.sunset  % 60));
  drawStringCenter(X, Y + 8 + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD) * 2, W, H, buf);
#else // Landscape
  char buf[32];
  snprintf(buf, sizeof(buf), "Sunrise / Sunset");
  GFX(setTextColor(COLOR_TITLE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  drawStringCenter(X, Y, W, H, buf);

  GFX(setTextColor(COLOR_VALUE));
  snprintf(buf, sizeof(buf), "%02d:%02d / %02d:%02d",
    (int)(data.sunrise / 60), (int)(data.sunrise % 60),
    (int)(data.sunset  / 60), (int)(data.sunset  % 60)
  );
  drawStringCenter(X, Y + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD), W, H, buf);
#endif

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Moon Phase
//---------------------------------------------------------------------------------------------
static void drawMoonPhase(int X, int Y, int W, int H, WeatherData &data) {
  struct tm tm;
  rtcConvertTime(data.time, &tm);

  float phase = calc_moon_phase(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
  const uint8_t *moon = get_phase_image(phase);
  if (moon) {
    GFX(drawGrayscaleBitmap(X + 16, Y, moon, MOON_WIDTH, MOON_HEIGHT));
  }

  GFX(setTextColor(COLOR_TITLE));
  GFX(setFont(&NotoSans_SemiBoldAlphabet7pt7b));
  GFX(setCursor(X + 12, Y + H - FONT_SMALL_HEIGHT + 8));
  GFX(print("Age"));

  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f", get_moon_age(phase));
  GFX(setTextColor(COLOR_VALUE));
  GFX(setCursor(X + 42, Y + H - FONT_SMALL_HEIGHT + 8));
  GFX(print(buf));

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Cloudiness, Air pressure
//---------------------------------------------------------------------------------------------
static void drawWeatherCondition(int X, int Y, int W, int H, WeatherData &data) {
#if (TFT_ROTATION == 0) || (TFT_ROTATION == 2)  // Portrait
  int y = 0;
#else // Landscape
  int y = 2;
#endif

  char buf[16];
  snprintf(buf, sizeof(buf), "Cloud");
  GFX(setTextColor(COLOR_TITLE));
  drawStringCenter(X, Y + y, W, H, buf);

  snprintf(buf, sizeof(buf), "%d %%", (int)data.weather[0].clouds);
  GFX(setTextColor(COLOR_VALUE));
  drawStringCenter(X, Y + y + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD), W, H, buf);

  snprintf(buf, sizeof(buf), "Rain");
  GFX(setTextColor(COLOR_TITLE));
  drawStringCenter(X, Y + y + 38, W, H, buf);

  snprintf(buf, sizeof(buf), "%d %%", (int)data.weather[0].rain);
  GFX(setTextColor(COLOR_VALUE));
  drawStringCenter(X, Y + y + 38 + (FONT_SMALL_HEIGHT + FONT_SMALL_LINEFD), W, H, buf);

  GFX_DBG(drawRect(X, Y, W, H, TFT_BLUE));
}

//---------------------------------------------------------------------------------------------
// Draw string text be center aligned
//---------------------------------------------------------------------------------------------
static void drawStringCenter(int16_t X, int16_t Y, int16_t W, int16_t H, const char *str) {
  int16_t x, y;
  uint16_t w, h;

  GFX(getTextBounds(str, 0, 0, &x, &y, &w, &h));
  GFX(setCursor((W ? X + (W - w) / 2 : X), (H ? Y + h : Y)));
  GFX(print(str));
}

//---------------------------------------------------------------------------------------------
// Search for the first array element of the next day
//---------------------------------------------------------------------------------------------
static int findNextDay(WeatherData &data, int n) {
  struct tm today;
  rtcConvertTime(data.weather[n].time, &today);

  for (int i = n + 1; i < data.n_weather; i++) {
    struct tm nextDay;
    rtcConvertTime(data.weather[i].time, &nextDay);
    if (nextDay.tm_wday != today.tm_wday) {
      return i;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------
static const char *getWeatherWind(uint8_t deg) {
  static const char *wind[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
  return wind[deg % 8];
}

//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------
static const char *getWeatherIcon(uint16_t weather_id, bool day) {
  switch (weather_id / 100) {
    case 2: return "H"; // thunderstorm
    case 3: return "F"; // drizzle
    case 5: switch (weather_id) {
      case 500: return "F"; // light rain
      case 501: return "J"; // moderate rain
      case 511: return "L"; // freezing rain
      default : return "K"; // rain
    }
    case 6:
      if (611 <= weather_id && weather_id <= 616) return "L"; // sleet
      return "G"; // snow
    case 7: return "B"; // fog
    case 8: {
      if (weather_id == 800) return (day ? "I" /* clear day         */ : "C" /* clear night         */);
      if (weather_id == 801) return (day ? "E" /* partly cloudy day */ : "D" /* partly cloudy night */);
      return "A"; // cloudy
    }
  }

  return "Z"; // unknown
}

//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------
static const char *getWeatherDescription(uint16_t weather_id) {
  switch (weather_id / 100) {
    case 2: return "Thunderstorm";
    case 3: return "Drizzle";
    case 5: return "Rain";
    case 6: return "Snow";
    case 7: switch (weather_id) {
      case 701: return "Mist";
      case 711: return "Smoke";
      case 721: return "Haze";
      case 731: return "Dust";
      case 741: return "Fog";
      case 751: return "Sand";
      case 761: return "Dust";
      case 762: return "Ash";
      case 771: return "Squall";
      case 781: return "Tornado";
      default : break;
    }
    case 8: switch (weather_id) {
      case 800: return "Clear";
      case 801:
      case 802:
      case 803:
      case 804: return "Clouds";
      default : break;
    }
  }

  return "????"; // unknown
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
static void parseWeatherData(JsonDocument &doc, WeatherData &data) {
  // Get updated time of day
  rtcCurrentTime(&data.time);

  // Get timezone
  data.timezone = (int32_t)doc["city"]["timezone"];

  // Get sunrise, Sunset
  struct tm tm;
  rtcConvertTime((int32_t)doc["city"]["sunrise"], &tm);
  data.sunrise = (uint16_t)(tm.tm_hour * 60 + tm.tm_min);

  rtcConvertTime((int32_t)doc["city"]["sunset"], &tm);
  data.sunset  = (uint16_t)(tm.tm_hour * 60 + tm.tm_min);

  // Get weather data
  #define MIN(a, b) ((a) < (b) ? (a) : (b))
  int N = doc["cnt"];
  N = MIN(N, sizeof(data.weather) / sizeof(data.weather[0]));

  int n = 0;
  for (JsonVariantConst list : doc["list"].as<JsonArrayConst>()) {
    data.weather[n].time        = (time32_t   )list["dt"];
    data.weather[n].temp        = (int16_t    )SCALE_MUL(float, list["main"]["temp" ], 10.0f);
    data.weather[n].wind_speed  = (uint16_t   )SCALE_MUL(float, list["wind"]["speed"], 10.0f);
    data.weather[n].wind_deg    = (uint8_t    )((SCALE_MUL(int, list["wind"]["deg"  ], 10) + 225) / 450) % 8;
    data.weather[n].humidity    = (uint8_t    )list["main"]["humidity"];
    data.weather[n].clouds      = (uint8_t    )list["clouds"]["all"];
    data.weather[n].rain        = (uint8_t    )SCALE_MUL(float, list["pop"], 100.0f);
    data.weather[n].id          = (uint16_t   )list["weather"][0]["id"];

    if (++n >= N) {
      break;
    }
  }

  data.n_weather = n;

#if DEBUG
  printWeatherData(data);
  Serial.print("Total: " ); Serial.print(n); // 40
  Serial.print(", Size: "); Serial.println(sizeof(data)); // 692
#endif
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
static void printWeatherData(WeatherData &data) {
  Serial.print("response: "); Serial.println(rtcTimeString(data.time));
  Serial.print("timezone: "); Serial.println(data.timezone);
  Serial.print("sunrise : "); Serial.println(data.sunrise);
  Serial.print("sunset  : "); Serial.println(data.sunset);

  for (int i = 0; i < data.n_weather; i++) {
    Serial.println(i);
    Serial.print("  date       : "); rtcPrintTime(data.weather[i].time);
    Serial.print("  time       : "); Serial.println((time32_t   )data.weather[i].time);
    Serial.print("  temp       : "); Serial.println((float      )data.weather[i].temp / 10.0f);
    Serial.print("  wind_speed : "); Serial.println((float      )data.weather[i].wind_speed / 10.0f);
    Serial.print("  wind_deg   : "); Serial.println((int        )data.weather[i].wind_deg);
    Serial.print("  humidity   : "); Serial.println((int        )data.weather[i].humidity);
    Serial.print("  clouds     : "); Serial.println((int        )data.weather[i].clouds);
    Serial.print("  rain       : "); Serial.println((int        )data.weather[i].rain);
    Serial.print("  id         : "); Serial.println((int        )data.weather[i].id);
  }
}