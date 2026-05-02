//=====================================================================================
// OpwnWeather application settings
//=====================================================================================
#pragma once

//-------------------------------------------------------------------------------------
// WiFi credentials and OpenWeather API key
// Please configure your `.gitignore` file appropriately 
// to prevent exposing "secret.h" on GitHub or others.
//-------------------------------------------------------------------------------------
#include "secrets.h"

//-------------------------------------------------------------------------------------
// OpenWeather settings
//-------------------------------------------------------------------------------------
#define HOST        "api.openweathermap.org"
#define PATH        "/data/2.5/forecast"
#define UNITS       "metric"  // celsius
#define LANGUAGE    "ja"      // language
#define LATITUDE    "35.69"   // latitude
#define LONGITUDE   "139.69"  // longitude

//-------------------------------------------------------------------------------------
// Update interval for requests to OpenWeatherMap and NTP server
//-------------------------------------------------------------------------------------
#define OWM_UPDATE_INTERVAL (10 * 60 * 1000LU)
#if defined(ARDUINO_UNOR4_WIFI)
  #define NTP_UPDATE_INTERVAL ( 1 * 60 * 1000LU)
#else
  #define NTP_UPDATE_INTERVAL ( 3 * 60 * 1000LU)
#endif

//-------------------------------------------------------------------------------------
// Timezone setting 
// See the definition of timezone environment variables:
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
//-------------------------------------------------------------------------------------
#define TIMEZONE_OFFSET   (9 * 3600)  // for UNO R4 WiFi (STD) [GMT+9]
#define TIMEZONE_STRING   "JST-9"     // for ESP32

//-------------------------------------------------------------------------------------
// Screen rotation (0 - 3)
//-------------------------------------------------------------------------------------
#if defined(ARDUINO_UNOR4_WIFI)
#define TFT_ROTATION  3
#else
#define TFT_ROTATION  0
#endif

//-------------------------------------------------------------------------------------
// TFT SPI setting
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