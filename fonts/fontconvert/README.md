# Arduino_GFX font converter 

This tool was extracted from Adafruit_GFX to generate font files for Arduino, **and has been customized for this application**.

Original software can be found at:
- https://github.com/moononournation/ArduinoFreeFontFile

You may see more details at Adafruit Learning System:
- https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

### How to use

1. Get the latest version of FreeType library from https://freetype.org/.

2. Compile `fontconvert` using gcc.

3. Converts TrueType font file to header file.
    ```
    ./fontconvert fontfile.ttf size [first ASCII code] [last ASCII code] > fontfile.h
    ```

## Temperature Font

The font files include the characters from ASCII code 32(Space) to 126(~), **but the bitmap data of ASCII code '!' is replaced to '°' (0xba)**.

## Numeric Font

The font files only inculded 13 charaters, ASCII code **46, 47 ('.', '/')**, 48-57(number 0-9) and followed ASCII code 58(':'). This can much reduce required program size if only want to use that font for displaying **numerical values** and clock time.

## Icon Font

- Weather icons: ASCII code from 65 ('A') to 79 ('Y')
- Wind icon: ASCII code from 48 ('0') to 55 ('7')
