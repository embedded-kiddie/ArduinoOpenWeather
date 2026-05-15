//=====================================================================================
// OpwnWeather application configuration
//=====================================================================================
#pragma once

//-------------------------------------------------------------------------------------
// WiFi credentials and OpenWeather API key
// Configure `.gitignore` appropriately to prevent exposing "secrets.h" on GitHub
//-------------------------------------------------------------------------------------
#include "secrets.h"

//-------------------------------------------------------------------------------------
// OpenWeather API
//-------------------------------------------------------------------------------------
#define HOST        "api.openweathermap.org"
#define PATH        "/data/2.5/forecast"
#define UNITS       "metric"  // celsius
#define LANGUAGE    "ja"      // language
#define LATITUDE    "35.69"   // latitude  (Tokyo)
#define LONGITUDE   "139.69"  // longitude (Tokyo)

//-------------------------------------------------------------------------------------
// Update interval for requests to OpenWeatherMap and NTP server
//-------------------------------------------------------------------------------------
#define HTTP_REQUEST_INTERVAL (10 * 60 * 1000LU)  // for OpenWeatherMap
#if defined(ARDUINO_UNOR4_WIFI)
  #define NTP_SYNC_INTERVAL   ( 1 * 60 * 1000LU)  // for NTP servr (UNO R4 WiFi)
#else
  #define NTP_SYNC_INTERVAL   ( 3 * 60 * 1000LU)  // for NTP servr (ESP32)
#endif

//-------------------------------------------------------------------------------------
// Timezone 
// See the definition of timezone environment variables:
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//-------------------------------------------------------------------------------------
#define TIMEZONE_OFFSET   (9 * 3600)  // for UNO R4 WiFi (STD) [GMT+9]
#define TIMEZONE_STRING   "JST-9"     // for ESP32

//-------------------------------------------------------------------------------------
// Screen Rotation (0 - 3)
//-------------------------------------------------------------------------------------
#if defined(ARDUINO_UNOR4_WIFI)
#define TFT_ROTATION  3
#else
#define TFT_ROTATION  0
#endif

//-------------------------------------------------------------------------------------
// SPI GPIO pins configuration for Arduino_GFX by moononournation
//-------------------------------------------------------------------------------------
#if defined(ARDUINO_UNOR4_WIFI)

// Arduino UNO R4 WiFi
#define TFT_DC        9
#define TFT_MISO      MISO  // 12
#define TFT_MOSI      MOSI  // 11
#define TFT_SCLK      SCK   // 13
#define TFT_CS        CS    // 10
#define TFT_RST       8
#define TFT_BL        -1

#else // ESP32

// ESP32-2432S028R (CYD)
#define TFT_DC        2
#define TFT_MISO      12
#define TFT_MOSI      13
#define TFT_SCLK      14
#define TFT_CS        15
#define TFT_RST       -1
#define TFT_BL        21

#endif