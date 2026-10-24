//=====================================================================================
// OpwnWeather application configuration
//=====================================================================================
#pragma once

//-------------------------------------------------------------------------------------
// WiFi credentials, OpenWeather API key and location
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
#ifndef LATITUDE              // if undefined in secrets.h
#define LATITUDE    "35.69"   // latitude  (Tokyo)
#define LONGITUDE   "139.69"  // longitude (Tokyo)
#endif

//-------------------------------------------------------------------------------------
// Timezone setting for NTP
// See the definition of timezone environment variables:
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//
// Note: Since UNO R4 RTC doesn't support timezone, manual configuration is required.
//-------------------------------------------------------------------------------------
#define NTP_TIMEZONE_DST    false       // Daylight Saving Time (UNO R4 WiFi)
#define NTP_TIMEZONE_OFFSET (9 * 3600)  // Time offset from UTC (UNO R4 WiFi)
#define NTP_TIMEZONE_STRING "JST-9"     // Posix timezone string (ESP32)

//-------------------------------------------------------------------------------------
// Update interval for requests to OpenWeather and NTP server
//-------------------------------------------------------------------------------------
#define HTTP_REQUEST_INTERVAL (10 * 60 * 1000LU)  // for OpenWeather
#if defined(ARDUINO_UNOR4_WIFI)
  #define NTP_SYNC_INTERVAL   ( 1 * 60 * 1000LU)  // for NTP servr (UNO R4 WiFi)
#else
  #define NTP_SYNC_INTERVAL   ( 3 * 60 * 1000LU)  // for NTP servr (ESP32)
#endif

//-------------------------------------------------------------------------------------
// Output Serial Monitor for debugging
//-------------------------------------------------------------------------------------
#define DEBUG false
#if DEBUG
#define DBG_EXEC(x) x
#else
#define DBG_EXEC(x)
#endif