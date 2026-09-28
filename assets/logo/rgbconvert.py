# Convert the RGB888 header file of an indexed color image exported by GIMP
# into RGB565.
#
# Operating conditions: GIMP 3.2.4, Python: 2.7.16

import re
import sys
import os


def rgb888_to_rgb565(r, g, b):
    return (
        ((r & 0xF8) << 8) |
        ((g & 0xFC) << 3) |
        (b >> 3)
    )


def generate_symbol_name(path):
    filename = os.path.basename(path)
    basename = os.path.splitext(filename)[0]

    # Convert non-alphanumeric characters to '_'.
    basename = re.sub(r'[^A-Za-z0-9]', '_', basename)

    # A C identifier cannot start with a digit.
    if basename and basename[0].isdigit():
        basename = '_' + basename

    return basename


def add_include_guard(text, symbol_base):
    guard = '_' + symbol_base.upper() + '_H_'

    header = (
        '#ifndef {}\n'
        '#define {}\n\n'
    ).format(guard, guard)

    footer = '\n#endif\n'

    return header + text + footer


def convert_width_height(text, symbol_base):
    # width
    width_pattern = re.compile(
        r'static\s+unsigned\s+int\s+width\s*=\s*(\d+)\s*;'
    )

    match = width_pattern.search(text)

    if match:
        width = match.group(1)

        replacement = '#define {}_WIDTH {}'.format(
            symbol_base.upper(),
            width
        )

        text = width_pattern.sub(replacement, text)

    # height
    height_pattern = re.compile(
        r'static\s+unsigned\s+int\s+height\s*=\s*(\d+)\s*;'
    )

    match = height_pattern.search(text)

    if match:
        height = match.group(1)

        replacement = '#define {}_HEIGHT {}'.format(
            symbol_base.upper(),
            height
        )

        text = height_pattern.sub(replacement, text)

    return text


def remove_header_pixel_macro(text):
    pattern = re.compile(
        r'/\*\s*Call this macro.*?'
        r'\*/\s*'
        r'#define\s+HEADER_PIXEL\s*\([^)]*\)\s*\{'
        r'.*?'
        r'data\s*\+\+\s*;\s*\}',
        re.DOTALL
    )

    return pattern.sub('', text)


def convert_header_data_cmap(text, symbol_base):
    # Change declaration to uint16_t.
    text = re.sub(
        r'static\s+unsigned\s+char\s+header_data_cmap\s*'
        r'\[\s*256\s*\]\s*\[\s*3\s*\]',
        'static const uint16_t {}_cmap[256]'.format(symbol_base),
        text
    )

    # Get the array body.
    pattern = re.compile(
        r'({}_cmap\s*\[\s*256\s*\]\s*=\s*\{{)(.*?)(\}};)'
        .format(re.escape(symbol_base)),
        re.DOTALL
    )

    match = pattern.search(text)

    if not match:
        return text

    start = match.group(1)
    body = match.group(2)
    end = match.group(3)

    # Extract RGB888 values.
    rgb_pattern = re.compile(
        r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\}'
    )

    rgb565_list = []

    for rgb_match in rgb_pattern.finditer(body):
        r = int(rgb_match.group(1))
        g = int(rgb_match.group(2))
        b = int(rgb_match.group(3))

        rgb565 = rgb888_to_rgb565(r, g, b)

        rgb565_list.append('0x{:04X}'.format(rgb565))

    # Format with 8 values per line.
    lines = []

    for i in range(0, len(rgb565_list), 8):
        chunk = rgb565_list[i:i + 8]
        lines.append('  ' + ', '.join(chunk))

    new_body = '\n' + ',\n'.join(lines) + '\n'

    converted = start + new_body + end

    return (
        text[:match.start()] +
        converted +
        text[match.end():]
    )


def convert_header_data(text, symbol_base):
    # Change declaration to uint8_t.
    text = re.sub(
        r'static\s+unsigned\s+char\s+header_data\s*\[\s*\]',
        'static const uint8_t {}[]'.format(symbol_base),
        text
    )

    pattern = re.compile(
        r'({}\s*\[\s*\]\s*=\s*\{{)(.*?)(\}};)'
        .format(re.escape(symbol_base)),
        re.DOTALL
    )

    match = pattern.search(text)

    if not match:
        return text

    start = match.group(1)
    body = match.group(2)
    end = match.group(3)

    numbers = re.findall(r'\d+', body)

    hex_list = []

    for n in numbers:
        hex_list.append('0x{:02X}'.format(int(n)))

    # Format with 16 values per line.
    lines = []

    for i in range(0, len(hex_list), 16):
        chunk = hex_list[i:i + 16]
        lines.append('  ' + ', '.join(chunk))

    new_body = '\n' + ',\n'.join(lines) + '\n'

    converted = start + new_body + end

    return (
        text[:match.start()] +
        converted +
        text[match.end():]
    )


def convert_file(input_path):
    input_file = open(input_path, 'r')
    text = input_file.read()
    input_file.close()

    # Generate symbol name from input filename.
    symbol_base = generate_symbol_name(input_path)

    # Remove the HEADER_PIXEL macro block.
    text = remove_header_pixel_macro(text)

    # width / height
    text = convert_width_height(text, symbol_base)

    # header_data_cmap
    text = convert_header_data_cmap(text, symbol_base)

    # header_data
    text = convert_header_data(text, symbol_base)

    # Include guard
    text = add_include_guard(text, symbol_base)

    # Output to stdout.
    sys.stdout.write(text)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("Usage:\n")
        sys.stderr.write("  python rgbconvert.py input.h\n")
        sys.exit(1)

    input_path = sys.argv[1]

    # Check whether the input file exists.
    if not os.path.isfile(input_path):
        sys.stderr.write(
            "Error: input file not found: {}\n".format(input_path)
        )
        sys.exit(1)

    # Trap exceptions that occur while processing the input file.
    try:
        convert_file(input_path)
    except Exception:
        sys.stderr.write(
            "Error: the contents of the input file are not appropriate.\n"
        )
        sys.exit(1)
