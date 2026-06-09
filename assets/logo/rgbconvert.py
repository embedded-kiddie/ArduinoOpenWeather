# Convert the RGB888 header file of an indexed color image exported by GIMP into RGB565.
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

    # Convert non-alphanumeric characters to '_'
    basename = re.sub(r'[^A-Za-z0-9]', '_', basename)

    return basename.upper()

def add_include_guard(text, symbol_base):
    guard = '_' + symbol_base + '_H_'
    header = (
        '#ifndef {}\n'
        '#define {}\n\n'
    ).format(guard, guard)
    footer = '\n#endif'

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
            symbol_base,
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
            symbol_base,
            height
        )

        text = height_pattern.sub(replacement, text)

    return text

def convert_header_data_cmap(text):
    # Change of declaration
    text = re.sub(
        r'static\s+unsigned\s+char\s+header_data_cmap\s*\[\s*256\s*\]\s*\[\s*3\s*\]',
        'static const unsigned short header_data_cmap[256]',
        text
    )

    # Get the array body
    pattern = re.compile(
        r'(header_data_cmap\s*\[256\]\s*=\s*\{)(.*?)(\};)',
        re.DOTALL
    )

    match = pattern.search(text)

    if not match:
        return text

    start = match.group(1)
    body = match.group(2)
    end = match.group(3)

    # Extract RGB
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

    # Format with 8 values per line
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


def convert_header_data(text):
    # Add 'const'
    text = re.sub(
        r'static\s+unsigned\s+char\s+header_data\s*\[\s*\]',
        'static const unsigned char header_data[]',
        text
    )

    pattern = re.compile(
        r'(header_data\s*\[\s*\]\s*=\s*\{)(.*?)(\};)',
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

    # Format with 16 values per line
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

def convert_file(input_path, output_path):
    input_file = open(input_path, 'r')
    text = input_file.read()
    input_file.close()

    # Generate symbol names
    input_symbol_base = generate_symbol_name(input_path)
    output_symbol_base = generate_symbol_name(output_path)

    # width / height
    text = convert_width_height(text, input_symbol_base)

    # header_data_cmap
    text = convert_header_data_cmap(text)

    # header_data
    text = convert_header_data(text)

    # include guard
    text = add_include_guard(text, output_symbol_base)

    output_file = open(output_path, 'w')
    output_file.write(text)
    output_file.close()

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage:")
        print("  python rgbconvert.py input.h output.h")
        sys.exit(1)

    convert_file(sys.argv[1], sys.argv[2])
    print("Converted successfully.")