//=====================================================================================
// A class definition for OpenWeather
// https://openweathermap.org/forecast5?collection=current_forecast
//=====================================================================================
#pragma once

#include <time.h>
#include <ArduinoJson.h>

// Scaling for data compression
#define SCALE_MUL(type, val, scale) ((type)(val) * (scale))
#define SCALE_DIV(type, val, scale) ((type)(val) / (scale))

//-------------------------------------------------------------------------------------
// Hourly weather data
//-------------------------------------------------------------------------------------
struct Weather {
  time32_t  time;         // 32bit unsigned long
  int16_t   temp;         // [deg]
  uint16_t  wind_speed;   // [m/s]
  uint8_t   wind_deg;     // [deg] (0 to 360)
  uint8_t   humidity;     // [%]
  uint8_t   clouds;       // [%]
  uint8_t   rain;         // [%]
  uint16_t  id;           // weather id (1xx to 8xx)
};

//-------------------------------------------------------------------------------------
// All weather data
//-------------------------------------------------------------------------------------
struct WeatherData {
  time32_t  time;         // updated time
  int32_t   timezone;     // offset from UTC+00
  uint16_t  sunrise;      // HH:MM --> HH * 60 + MM
  uint16_t  sunset;       // HH:MM --> HH * 60 + MM
  int32_t   n_weather;    // number of weather data
  Weather   weather[40];  // 8/day x 5
};

//-------------------------------------------------------------------------------------
// Class definition
//-------------------------------------------------------------------------------------
class OpenWeather {
public:
  OpenWeather(void) {}
  ~OpenWeather(void) {}

  // Responsed weather data in JSON
  JsonDocument data;

  void Init(void);
  bool RequestWeatherData(void);

private:
  bool readResponse(Stream &stream);
};