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
// https://github.com/arduino/ArduinoCore-renesas/blob/main/variants/UNOWIFIR4/pins_arduino.h
// TFT_RST and TFT_DC should be configured based on your wiring
#define TFT_RST       D8    //  8
#define TFT_DC        D9    //  9
#define TFT_CS        CS    // 10
#define TFT_MOSI      MOSI  // 11
#define TFT_MISO      MISO  // 12
#define TFT_SCLK      SCK   // 13
#define TFT_BL        -1    // Connect directly to 3.3V
#define SPI_FREQUENCY       // Default for UNO R4

#else // ESP32

// ESP32-2432S028R (CYD)
// https://github.com/espressif/arduino-esp32/blob/master/variants/jczn_2432s028r/pins_arduino.h
#define TFT_RST       -1
#define TFT_DC        2
#define TFT_MISO      12
#define TFT_MOSI      13
#define TFT_SCLK      14
#define TFT_CS        15
#define TFT_BL        21
#define SPI_FREQUENCY 80000000  // ILI9341 for ESP32 (40MHz or 80MHz)

#endif // ARDUINO_UNOR4_WIFI or ESP32

//-------------------------------------------------------------------------------------
// Output Serial Monitor for debugging
//-------------------------------------------------------------------------------------
#define DEBUG false
#if DEBUG
#define DBG_EXEC(x) x
#else
#define DBG_EXEC(x)
#endif