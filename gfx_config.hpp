//=====================================================================================
// GPIO pin assignment and Graphic Library Configuration for TFT LCDs
//
// Target MCU dev board:
//  - UNO R4 Minima, UNO R4 WiFi, Nano R4
//  - ESP32-2432S028R (CYD)
// Target Graphic library:
//  - Adafruit GFX (ILI9341, ST7789)
//  - Arduino GFX (ILI9341, ILI9486, ST7789, ST7796)
//
// Usage:
//  // 93410 and 77890 are for 8-bit parallel MCUFRIEND-type LCDs.
//  #define USE_ADAFRUIT    false // Select library: true or false
//  #define USE_TFT_DRIVER  9341  // Select Driver : 9341[0], 7789[0], 9486, 7796
//  #define USE_OPERATION   2     // Init operation: type 1 or 2 for 9341 and 7789
//  #inclue "gfx_config.h"
//=====================================================================================
#ifndef _GFX_CONFIG_HPP_
#define _GFX_CONFIG_HPP_

//-------------------------------------------------------------------------------------
// SPI GPIO pin assignment
//-------------------------------------------------------------------------------------
#if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
  // Arduino UNO R4 WiFi
  // https://github.com/arduino/ArduinoCore-renesas/blob/main/variants/UNOWIFIR4/pins_arduino.h
  // TFT_RST and TFT_DC should be configured based on your wiring
  #define TFT_RST       8         // D8
  #define TFT_DC        9         // D9
  #define TFT_CS        10        // CS
  #define TFT_MOSI      MOSI      // 11
  #define TFT_MISO      MISO      // 12
  #define TFT_SCLK      SCK       // 13
  #define TFT_BL        -1        // Connect directly to 3.3V
  #define SPI_FREQUENCY 24000000  // RA4M1: 24MHz, SPI: 12MHz

#else  // ESP32
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
#endif

//-------------------------------------------------------------------------------------
// Initial rotation and panel size
//-------------------------------------------------------------------------------------
#ifndef TFT_ROTATION
#define TFT_ROTATION  0
#endif

#ifndef TFT_WIDTH
  #if (USE_TFT_DRIVER == 9486) || (USE_TFT_DRIVER == 7796)
    #define TFT_WIDTH     (TFT_ROTATION & 1 ? 480 : 320)
    #define TFT_HEIGHT    (TFT_ROTATION & 1 ? 320 : 480)
  #else
    #define TFT_WIDTH     (TFT_ROTATION & 1 ? 320 : 240)
    #define TFT_HEIGHT    (TFT_ROTATION & 1 ? 240 : 320)
  #endif
#endif

//-----------------------------------------------------------------
// Initial Operation
// Fix color misregistration compared to Adafruit GFX
// https://github.com/moononournation/Arduino_GFX/discussions/820
//-----------------------------------------------------------------
#if (USE_OPERATION == 1)
  // Library default configuration
  #define GFX_CONFIG(bus, rst, rot) bus, rst, rot

#elif (USE_TFT_DRIVER == 9341)
  #define GFX_CONFIG(bus, rst, rot) bus, rst, rot, \
    false /* ips */, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, \
    0 /* col offset 1 */,  0 /* row offset 1 */,  0 /* col offset 2 */,  0 /* row offset 2 */, \
    ili9341_type2_init_operations, sizeof(ili9341_type2_init_operations)

#elif (USE_TFT_DRIVER == 7789)
  // The library's default configuration does not produce the correct colors
  #define GFX_CONFIG(bus, rst, rot) bus, rst, rot, \
    false /* ips */, ST7789_TFTWIDTH, ST7789_TFTHEIGHT, \
    0 /* col offset 1 */,  0 /* row offset 1 */,  0 /* col offset 2 */,  0 /* row offset 2 */, \
    st7789_type2_init_operations, sizeof(st7789_type2_init_operations)

#else
  #define GFX_CONFIG(bus, rst, rot) bus, rst, rot
#endif

//-----------------------------------------------------------------
// Data bus class
//-----------------------------------------------------------------
#if defined(ARDUINO_ARCH_RENESAS_UNO) || defined(ARDUINO_ARCH_AVR)
  #if (USE_TFT_DRIVER == 9341)
    Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);

  #elif (USE_TFT_DRIVER == 7789)
    #if defined(ARDUINO_ARCH_RENESAS_UNO)
      Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, /* TFT_MISO */ GFX_NOT_DEFINED);
    #else
      Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS); // Display glitches or nothing
    #endif

  #else
    // MCUFRIEND type display breakout board for Arduino UNO and Mega2560
    Arduino_DataBus *bus = new Arduino_UNOPAR8();
  #endif

#else // ESP32
//Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);    // 4-wires (8-bit) SPI (slow)
  Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO); // 4-wires (8-bit) SPI (fast)
//Arduino_DataBus *bus = new Arduino_ESP32SPI(GFX_NOT_DEFINED, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO); // 3-wires (9bit) SPI
#endif

//-----------------------------------------------------------------
// Display class
//-----------------------------------------------------------------
#if (USE_TFT_DRIVER == 9341)
  // 2.4/2.8inch SPI Module ILI9341
  // https://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
  // https://www.lcdwiki.com/2.8inch_SPI_Module_ILI9341_SKU:MSP2807
  Arduino_GFX *gfx = new Arduino_ILI9341(GFX_CONFIG(bus, TFT_RST, TFT_ROTATION));

#elif (USE_TFT_DRIVER == 7789)
  Arduino_GFX *gfx = new Arduino_ST7789(GFX_CONFIG(bus, TFT_RST, TFT_ROTATION));

#elif (USE_TFT_DRIVER == 9486)
  // 3.5inch 8-bit parallel ILI9486
  // https://www.lcdwiki.com/3.5inch_Arduino_Display-UNO
  Arduino_GFX *gfx = new Arduino_ILI9486(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

#elif (USE_TFT_DRIVER == 7796)
  // 3.5inch 8-bit parallel ST7789
  // https://www.lcdwiki.com/3.5inch_Arduino_Display-UNO
  Arduino_GFX *gfx = new Arduino_ST7796(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

#elif (USE_TFT_DRIVER == 93410)
  // 2.4inch 8-bit parallel ILI9341
  // https://www.lcdwiki.com/2.4inch_Arduino_Display
  Arduino_GFX *gfx = new Arduino_ILI9341(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));

#elif (USE_TFT_DRIVER == 77890)
  // 2.4inch 8-bit parallel ILI9341
  // https://www.lcdwiki.com/2.4inch_Arduino_Display
  Arduino_GFX *gfx = new Arduino_ST7789(GFX_CONFIG(bus, A4, (TFT_ROTATION + 2) & 3));
#endif

//-----------------------------------------------------------------
// GFX function
//-----------------------------------------------------------------
#define GFX(f)    gfx->f
#define GFX_NAME  "Arduino GFX"

void gfx_setup(void) {
  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }

  if (!GFX(begin(SPI_FREQUENCY))) {
    Serial.println("gfx->begin() failed!");
  }
}

#endif // _GFX_CONFIG_HPP_