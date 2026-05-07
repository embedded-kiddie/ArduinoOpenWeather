# ArduinoOpenWeather

## What is this?

![OpenWeather for UNO R4 WiFi](assets/OpenWeather_UNO_R4.jpg)

This is yet another OpenWeatherMap app for UNO R4 WiFi and ESP32.

Inspired by the cool screen design of [OpenWeahter][1] by [Bodmer, the creator of TFT_eSPI][2], I migrated it for my UNO R4 WiFi.

## Software Requirements

### Graphics library
- [moononournation/Arduino_GFX][3] v1.6.5

### UNO R4 WiFi

| Package                | Version                                     |
| ---------------------- | ------------------------------------------- |
| Board Platform         | [Arduino UNO R4 Boards by Arduino][4] 1.5.3 |
| NTP Client             | [NTPClient][5] 3.2.1                        |
| HTTP Client (optional) | [ArduinoHttpClient][6] 0.6.1                |

### ESP32

| Package        | Version                               |
| -------------- | ------------------------------------- |
| Board Platform | [esp32 by Espressif Systems][7] 3.3.8 |

## Software Configuration

1. Open [`config.h`][8] and and configure the following:

    | Item                  | Configuration Parameters                                             |
    | --------------------- | -------------------------------------------------------------------- |
    | **OpenWeather API**   | `LANGUAGE`, `LATITUDE` and `LONGITUDE`                               |
    | **Timezone**          | `TIMEZONE_OFFSET` (for UNO R4 WiFi) or `TIMEZONE_STRING` (for ESP32) |
    | **Screen Rotation**   | `TFT_ROTATION` (0,2: portrait / 1,3: landscape)                      |
    | **SPI GPIO pins** | `TFT_DC`, `TFT_MISO`, `TFT_MOSI`, ...                                |

2. Create your `secrets.h` for WiFi credential and OpenWeather API key:

    ```c++
    #define SECRET_SSID "***** Your SSID *****"
    #define SECRET_PASS "***** Your password *****"
    #define API_KEY     "***** your OpenWeather key *****"
    ```

3. Open [`gfx.cpp`][9] and modify the Arduino_GFX code to suit your LCD device:

    ```c++
    #if defined(ARDUINO_UNOR4_WIFI)
    Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
    Arduino_GFX *tft = new Arduino_ILI9341(bus, TFT_RST);
    #else // ESP32
    Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);
    Arduino_GFX *tft = new Arduino_ILI9341(bus, TFT_RST);
    #endif

    #define SPI_FREQ    40000000  // or 80000000
    ```

4. Experimental Feature

    | Symbol                 | Default           |
    | ---------------------- | ----------------- |
    | `USE_SECURE_CLIENT`    | `true`            |
    | `USE_HTTP_CLIENT`      | `false`           |
    | `DESERIALIZATION_TYPE` | `TYPE_RAW_STREAM` |

## ToDo List

- [ ] Fix the day of the week from 21:00 through midnight.
- [ ] Display progress on the opening splash screen.
- [ ] Make weather icons multi-colorizing.
- [ ] Verify Daylight Saving Time operation.
- [ ] Supress debug printing to the Serial Monitor.

## Acknowledgement

### Icon Fonts

I would like to express my gratitude to the creators of these cool icon fonts:

- [kickstandapps/WeatherIcons][10] licensed under the [SIL OPEN FONT LICENSE Version 1.1][12].
- [erikflowers/weather-icons][11] licensed under the [SIL OPEN FONT LICENSE Version 1.1][12].


[1]: https://github.com/Bodmer/OpenWeather "Bodmer/OpenWeather: Arduino library to fetch weather information from OpenWeather"

[2]: https://github.com/Bodmer/TFT_espi "Bodmer/TFT_eSPI: Arduino and PlatformIO IDE compatible TFT library optimised for the Raspberry Pi Pico (RP2040), STM32, ESP8266 and ESP32 that supports different driver chips"

[3]: https://github.com/moononournation/Arduino_GFX "moononournation/Arduino_GFX: Arduino GFX developing for various color displays and various data bus interfaces"

[4]: https://github.com/arduino/ArduinoCore-renesas "arduino/ArduinoCore-renesas"

[5]: https://github.com/arduino-libraries/NTPClient "arduino-libraries/NTPClient: Connect to a NTP server"

[6]: https://github.com/arduino-libraries/ArduinoHttpClient "arduino-libraries/ArduinoHttpClient: Arduino HTTP Client library"

[7]: https://github.com/espressif/arduino-esp32 "espressif/arduino-esp32: Arduino core for the ESP32"

[8]: config.h
[9]: gfx.cpp

[10]: https://github.com/kickstandapps/WeatherIcons "kickstandapps/WeatherIcons: Open-sourced weather icons and font for UI design."

[11]: https://github.com/erikflowers/weather-icons "erikflowers/weather-icons: 215 Weather Themed Icons and CSS"

[12]: https://openfontlicense.org/open-font-license-official-text/ "SIL Open Font License Official Text"
