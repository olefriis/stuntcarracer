#!/usr/bin/env python3
"""Extract all graphics from Stunt Car Racer ADF disk image.

Extracts:
  1. All 52 graphic.info entries from the pre-baked screen image
     (wheels, flames, exhaust, corners, flags, chains, holes, smashes,
      damage bar, dust clouds, sprites, message panel)
  2. Entries 10-14 from the car.crunched screen (engine block, exhausts, corners)
  3. Nine RLE-compressed full-screen images (car, title, preview, hallfame,
     people, wreck, won, lost, promotion)

Screen format: word-interleaved 4 bitplanes, 320px wide, 160 bytes/line.
Each 8-byte group = 4 planes × 2 bytes (16 pixels per plane).

RLE format (R.59450): 200 lines × 4 planes. Each plane = 40 bytes/line.
  Negative byte → RLE run (repeat next byte N times)
  Positive byte → literal copy (N+1 bytes)
  Output is interleaved: write 2 bytes, skip 6, write 2, skip 6...
"""

import os
import struct
import zlib

ADF_FILE = 'stunt-car-racer.adf'
OUTPUT_DIR = 'extracted-images'

# screen1.space at ADF offset 0x69A94
# First 2 bytes: resolution(?), next 32 bytes: palette, then bitmap
SCREEN1_OFFSET = 0x69A94
SCREEN_BITMAP_OFFSET = SCREEN1_OFFSET + 34  # = 0x69AB6
BYTES_PER_LINE = 160  # 4 planes × 40 bytes/plane, word-interleaved

# ---------------------------------------------------------------------------
# Palettes (Amiga $0RGB, 4 bits per channel)
# ---------------------------------------------------------------------------

PALETTES = {
    'car': [
        0x000, 0x443, 0x554, 0x770, 0x451, 0x233, 0x257, 0x247,
        0x123, 0x200, 0x311, 0x422, 0x644, 0x332, 0x555, 0x777,
    ],
    'title': [
        0x000, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x240, 0x021, 0x046, 0x025, 0x710, 0x500, 0x740,
    ],
    'preview': [
        0x022, 0x443, 0x554, 0x770, 0x123, 0x222, 0x030, 0x247,
        0x000, 0x200, 0x311, 0x050, 0x555, 0x332, 0x333, 0x777,
    ],
    'hallfame': [
        0x000, 0x221, 0x332, 0x443, 0x034, 0x110, 0x030, 0x770,
        0x000, 0x200, 0x311, 0x070, 0x555, 0x221, 0x333, 0x777,
    ],
    'people': [
        0x222, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x240, 0x030, 0x035, 0x025, 0x710, 0x500, 0x740,
    ],
    'wreck': [
        0x000, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x230, 0x021, 0x046, 0x025, 0x710, 0x500, 0x740,
    ],
    'won': [
        0x000, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x230, 0x021, 0x046, 0x025, 0x710, 0x500, 0x740,
    ],
    'lost': [
        0x000, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x230, 0x021, 0x046, 0x025, 0x710, 0x500, 0x740,
    ],
    'promotion': [
        0x000, 0x777, 0x555, 0x222, 0x000, 0x743, 0x632, 0x421,
        0x310, 0x230, 0x021, 0x046, 0x025, 0x710, 0x500, 0x740,
    ],
}

# Sprite palette (from assembly: sprite.colours)
SPRITE_PALETTE_AMIGA = [0x000, 0x000, 0xfff, 0xc88]

# ADF offsets for crunched (RLE-compressed) full-screen images
# Each has 32-byte palette immediately before the compressed data
CRUNCHED_IMAGES = {
    'car':       0x1f6a4,
    'title':     0x23152,
    'preview':   0x2ae94,
    'hallfame':  0x300f0,
    'people':    0x34f96,
    'wreck':     0x3ccd8,
    'won':       0x43c9e,
    'lost':      0x4a460,
    'promotion': 0x5036e,
}

# ---------------------------------------------------------------------------
# graphic.info table: 52 entries
# Each entry: (x_words, y, width_words_minus1, height_minus1, name, comment)
# Entries 37-48 are sprites (2-bitplane hardware sprite format)
# Entries 10-14 are extracted from car.crunched decompressed screen
# ---------------------------------------------------------------------------

GRAPHIC_ENTRIES = [
    # idx  x   y   w-1  h-1   name                    category
    (0,    0,  0,  1,   57,  'right-wheel-0',         'wheels'),
    (1,    2,  0,  1,   57,  'right-wheel-1',         'wheels'),
    (2,    4,  0,  1,   57,  'right-wheel-2',         'wheels'),
    (3,    8,  0,  1,   57,  'left-wheel-0',          'wheels'),
    (4,   10,  0,  1,   57,  'left-wheel-1',          'wheels'),
    (5,   12,  0,  1,   57,  'left-wheel-2',          'wheels'),
    (6,    0, 68,  3,   27,  'left-flame-0',          'flames'),
    (7,    4, 68,  3,   27,  'left-flame-1',          'flames'),
    (8,    8, 68,  3,   27,  'right-flame-0',         'flames'),
    (9,   12, 68,  3,   27,  'right-flame-1',         'flames'),
    (10,   2,123, 15,   20,  'engine-block',          'cockpit'),  # from car.crunched
    (11,   2,144,  1,   14,  'exhaust-left',          'cockpit'),  # from car.crunched
    (12,  16,144,  1,   14,  'exhaust-right',         'cockpit'),  # from car.crunched
    (13,   2, 16,  0,    5,  'corner-left',           'cockpit'),  # from car.crunched
    (14,  17, 16,  0,    5,  'corner-right',          'cockpit'),  # from car.crunched
    (15,  14,  0,  0,    7,  'flag-bright',           'indicators'),
    (16,  14,  8,  0,    7,  'flag-dull',             'indicators'),
    (17,  14, 16,  0,    7,  'stopwatch-bright',      'indicators'),
    (18,  14, 24,  0,    7,  'stopwatch-dull',        'indicators'),
    (19,  15,  0,  0,    7,  'chain-left-top',        'chains'),
    (20,  15,  8,  0,    7,  'chain-left-bottom',     'chains'),
    (21,  15, 16,  0,    7,  'chain-right-top',       'chains'),
    (22,  15, 24,  0,    7,  'chain-right-bottom',    'chains'),
    (23,  16,  0,  1,    7,  'hole-1',                'damage'),
    (24,  16,  8,  1,    7,  'hole-2',                'damage'),
    (25,  16, 16,  1,    7,  'smash-1',               'damage'),
    (26,  16, 24,  1,    7,  'smash-2',               'damage'),
    (27,  16, 32,  1,    7,  'damage-bar-clear-1',    'damage'),
    (28,  16, 40,  1,    7,  'damage-bar-clear-2',    'damage'),
    (29,   0, 96,  3,   33,  'dust-cloud-0',          'dust'),
    (30,   5, 96,  3,   30,  'dust-cloud-1',          'dust'),
    (31,   9, 96,  3,   37,  'dust-cloud-2',          'dust'),
    (32,  13, 96,  4,   35,  'dust-cloud-3',          'dust'),
    (33,   0,134,  2,   27,  'dust-cloud-4',          'dust'),
    (34,   3,134,  3,   33,  'dust-cloud-5',          'dust'),
    (35,   7,134,  3,   33,  'dust-cloud-6',          'dust'),
    (36,  11,134,  3,   35,  'dust-cloud-7',          'dust'),
    (37,   1,176,  0,   15,  'sprite-0',              'sprites'),
    (38,   3,176,  0,   15,  'sprite-1',              'sprites'),
    (39,   5,176,  0,   15,  'sprite-2',              'sprites'),
    (40,   8,176,  0,   15,  'sprite-3',              'sprites'),
    (41,  10,176,  0,   15,  'sprite-4',              'sprites'),
    (42,  12,176,  0,   15,  'sprite-5',              'sprites'),
    (43,   0,176,  0,   15,  'sprite-6',              'sprites'),
    (44,   2,176,  0,   15,  'sprite-7',              'sprites'),
    (45,   4,176,  0,   15,  'sprite-8',              'sprites'),
    (46,   7,176,  0,   15,  'sprite-9',              'sprites'),
    (47,   9,176,  0,   15,  'sprite-10',             'sprites'),
    (48,  11,176,  0,   15,  'sprite-11',             'sprites'),
    (49,  16, 68,  3,   27,  'left-flame-2',          'flames'),
    (50,  16,172,  3,   27,  'right-flame-2',         'flames'),
    (51,  16,134,  3,   27,  'message-panel',         'misc'),
]


def amiga_brighten(n):
    """Apply the set.amiga.colours brightness transformation to a 4-bit channel.

    The game stores palette nibbles in the range 0-7 and the display routine
    (R.set.amiga.colours) doubles each channel and sets the LSB if non-zero:
      f(0) = 0, f(n) = 2n + 1  for n >= 1
    This maps 0-7 to approximately 0-F.
    """
    if n == 0:
        return 0
    return min(2 * n + 1, 0xF)


def amiga_palette_to_rgb(amiga_colors, brighten=True):
    """Convert list of Amiga $0RGB values to list of (R8, G8, B8) tuples.

    If brighten is True (default), applies the set.amiga.colours transformation
    that the game uses before writing to hardware registers. Sprite palettes
    bypass this transformation and should pass brighten=False.
    """
    rgb = []
    for c in amiga_colors:
        r = (c >> 8) & 0xF
        g = (c >> 4) & 0xF
        b = c & 0xF
        if brighten:
            r = amiga_brighten(r)
            g = amiga_brighten(g)
            b = amiga_brighten(b)
        rgb.append((r * 17, g * 17, b * 17))
    return rgb


def decode_word_interleaved(data, screen_offset, x_words, y, width_words, height):
    """Decode a rectangular region from a word-interleaved 4-plane screen.

    Each 8-byte group on screen:
      bytes 0-1: plane 0 (16 pixels, bit 15 = leftmost)
      bytes 2-3: plane 1
      bytes 4-5: plane 2
      bytes 6-7: plane 3

    Returns 2D list of 4-bit palette indices.
    """
    pixels = []
    for line in range(height):
        row = []
        line_offset = screen_offset + (y + line) * BYTES_PER_LINE
        for grp in range(width_words):
            grp_offset = line_offset + (x_words + grp) * 8
            p0 = struct.unpack('>H', data[grp_offset:grp_offset+2])[0]
            p1 = struct.unpack('>H', data[grp_offset+2:grp_offset+4])[0]
            p2 = struct.unpack('>H', data[grp_offset+4:grp_offset+6])[0]
            p3 = struct.unpack('>H', data[grp_offset+6:grp_offset+8])[0]
            for bit in range(15, -1, -1):
                idx = 0
                if p0 & (1 << bit): idx |= 1
                if p1 & (1 << bit): idx |= 2
                if p2 & (1 << bit): idx |= 4
                if p3 & (1 << bit): idx |= 8
                row.append(idx)
        pixels.append(row)
    return pixels


def decode_sprite(data, screen_offset, x_words, y, height):
    """Decode a sprite from the screen (16px wide, 2 bitplanes: planes 0 and 2).

    Matches the R.69e30 routine which reads plane 0 (inverted) and plane 2
    to create Amiga hardware sprite data. Produces 2-bit color indices
    into the sprite palette.
    """
    pixels = []
    for line in range(height):
        row = []
        line_offset = screen_offset + (y + line) * BYTES_PER_LINE
        grp_offset = line_offset + x_words * 8
        p0 = struct.unpack('>H', data[grp_offset:grp_offset+2])[0]
        p2 = struct.unpack('>H', data[grp_offset+4:grp_offset+6])[0]
        # R.69e30 inverts plane 0
        p0 = p0 ^ 0xFFFF
        for bit in range(15, -1, -1):
            idx = 0
            if p0 & (1 << bit): idx |= 1
            if p2 & (1 << bit): idx |= 2
            row.append(idx)
        pixels.append(row)
    return pixels


def decompress_rle(data, src_offset):
    """Decompress RLE-compressed screen data (R.59450).

    Produces a 32000-byte word-interleaved 4-bitplane screen buffer
    (200 lines × 160 bytes/line).

    The compressor works per-line, per-plane:
      - 200 lines, 4 planes per line, 40 bytes per plane per line
      - Output is interleaved: write 2 bytes, skip 6 (plane stride)
      - Negative source byte: RLE run (repeat next byte N times)
      - Positive source byte: literal copy (N+1 bytes)
    """
    screen = bytearray(32000)  # 200 lines × 160 bytes
    src = src_offset
    dst_base = 0  # start of current line

    for line in range(200):
        for plane in range(4):
            dst_plane = dst_base + plane * 2  # plane offset within line
            bytes_written = 0

            while bytes_written < 40:
                cmd = data[src]
                src += 1

                if cmd & 0x80:  # negative (signed)
                    count = 256 - cmd  # negate
                    if count == 128:
                        continue  # -128 is a no-op (bmi .label3)
                    # dbra loops count+1 times (decrement-and-branch)
                    val = data[src]
                    src += 1
                    for _ in range(count + 1):
                        # Write one byte at interleaved position
                        pos = dst_plane + (bytes_written // 2) * 8 + (bytes_written & 1)
                        screen[pos] = val
                        bytes_written += 1
                else:
                    count = cmd + 1
                    for _ in range(count):
                        val = data[src]
                        src += 1
                        pos = dst_plane + (bytes_written // 2) * 8 + (bytes_written & 1)
                        screen[pos] = val
                        bytes_written += 1

        dst_base += 160  # next screen line

    return bytes(screen)


def decode_st_sequential(data, src_offset):
    """Decode raw Atari ST sequential bitplane screen data.

    The source has 4 words per 16-pixel group: [p0, p1, p2, p3],
    stored sequentially across the screen. The copy loop distributes
    them into 4 planes of 8000 bytes each (40 bytes/line × 200 lines).

    Converts to word-interleaved format (same as decompress_rle output)
    so it can be fed to decode_word_interleaved / pixels_from_screen.
    """
    screen = bytearray(32000)  # 200 lines × 160 bytes, word-interleaved
    src = src_offset

    # Source: 4000 groups of 4 words (p0, p1, p2, p3)
    # Destination planes: each 8000 bytes at offsets 0, 8000, 16000, 24000
    # We need to convert sequential planes to word-interleaved
    planes = [bytearray(8000) for _ in range(4)]
    for i in range(4000):
        for p in range(4):
            planes[p][i * 2] = data[src]
            planes[p][i * 2 + 1] = data[src + 1]
            src += 2

    # Convert sequential planes to word-interleaved
    for line in range(200):
        for word in range(20):  # 20 word-groups per line (320px / 16)
            dst = line * 160 + word * 8
            plane_offset = line * 40 + word * 2
            for p in range(4):
                screen[dst + p * 2] = planes[p][plane_offset]
                screen[dst + p * 2 + 1] = planes[p][plane_offset + 1]

    return bytes(screen)


# Images that use raw ST sequential format instead of RLE compression
ST_SEQUENTIAL_IMAGES = {'title', 'people'}


def write_png(filename, pixels, palette, width, height, transparent_color=0):
    """Write an RGBA PNG. The specified color index is transparent."""
    def make_chunk(chunk_type, chunk_data):
        chunk = chunk_type + chunk_data
        crc = zlib.crc32(chunk) & 0xFFFFFFFF
        return struct.pack('>I', len(chunk_data)) + chunk + struct.pack('>I', crc)

    sig = b'\x89PNG\r\n\x1a\n'
    ihdr = make_chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0))

    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter: none
        for x in range(width):
            idx = pixels[y][x]
            if idx == transparent_color:
                raw.extend([0, 0, 0, 0])
            else:
                r, g, b = palette[idx]
                raw.extend([r, g, b, 255])

    compressed = zlib.compress(bytes(raw))
    idat = make_chunk(b'IDAT', compressed)
    iend = make_chunk(b'IEND', b'')

    with open(filename, 'wb') as f:
        f.write(sig + ihdr + idat + iend)


def write_png_opaque(filename, pixels, palette, width, height):
    """Write an RGB PNG (no transparency)."""
    def make_chunk(chunk_type, chunk_data):
        chunk = chunk_type + chunk_data
        crc = zlib.crc32(chunk) & 0xFFFFFFFF
        return struct.pack('>I', len(chunk_data)) + chunk + struct.pack('>I', crc)

    sig = b'\x89PNG\r\n\x1a\n'
    ihdr = make_chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0))

    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter: none
        for x in range(width):
            idx = pixels[y][x]
            r, g, b = palette[idx]
            raw.extend([r, g, b])

    compressed = zlib.compress(bytes(raw))
    idat = make_chunk(b'IDAT', compressed)
    iend = make_chunk(b'IEND', b'')

    with open(filename, 'wb') as f:
        f.write(sig + ihdr + idat + iend)


def pixels_from_screen(screen_data, screen_offset, x_words, y, width_words, height):
    """Helper to get pixel grid from a screen buffer at a given offset."""
    return decode_word_interleaved(screen_data, screen_offset, x_words, y, width_words, height)


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    adf_path = os.path.join(script_dir, ADF_FILE)
    output_dir = os.path.join(script_dir, OUTPUT_DIR)

    with open(adf_path, 'rb') as f:
        adf_data = f.read()

    print(f"ADF size: {len(adf_data)} bytes")
    os.makedirs(output_dir, exist_ok=True)

    car_palette = amiga_palette_to_rgb(PALETTES['car'])
    sprite_palette = amiga_palette_to_rgb(SPRITE_PALETTE_AMIGA, brighten=False)

    # -----------------------------------------------------------------------
    # Decompress car.crunched for entries 10-14
    # -----------------------------------------------------------------------
    print("\nDecompressing car.crunched...")
    car_screen = decompress_rle(adf_data, CRUNCHED_IMAGES['car'])

    # -----------------------------------------------------------------------
    # Extract all 52 graphic.info entries
    # -----------------------------------------------------------------------
    print("\n--- Graphic entries ---")
    for idx, x, y, w_m1, h_m1, name, category in GRAPHIC_ENTRIES:
        width_words = w_m1 + 1
        height = h_m1 + 1
        width_px = width_words * 16

        cat_dir = os.path.join(output_dir, category)
        os.makedirs(cat_dir, exist_ok=True)
        filename = os.path.join(cat_dir, f'{name}.png')

        if 37 <= idx <= 48:
            # Sprites: 2-bitplane extraction with sprite palette
            pixels = decode_sprite(adf_data, SCREEN_BITMAP_OFFSET, x, y, height)
            write_png(filename, pixels, sprite_palette, width_px, height)
            print(f"  [{idx:2d}] {name}: {width_px}x{height} (sprite) -> {category}/{name}.png")

        elif 10 <= idx <= 14:
            # Entries from car.crunched decompressed screen
            pixels = pixels_from_screen(car_screen, 0, x, y, width_words, height)
            write_png(filename, pixels, car_palette, width_px, height, transparent_color=1)
            print(f"  [{idx:2d}] {name}: {width_px}x{height} (car.crunched) -> {category}/{name}.png")

        else:
            # Standard entries from pre-baked screen
            pixels = pixels_from_screen(adf_data, SCREEN_BITMAP_OFFSET, x, y, width_words, height)
            write_png(filename, pixels, car_palette, width_px, height, transparent_color=1)
            print(f"  [{idx:2d}] {name}: {width_px}x{height} -> {category}/{name}.png")

    # -----------------------------------------------------------------------
    # Extract all crunched full-screen images (320×200)
    # -----------------------------------------------------------------------
    print("\n--- Full-screen images ---")
    screens_dir = os.path.join(output_dir, 'screens')
    os.makedirs(screens_dir, exist_ok=True)

    for name, data_offset in CRUNCHED_IMAGES.items():
        palette = amiga_palette_to_rgb(PALETTES[name])
        if name in ST_SEQUENTIAL_IMAGES:
            screen = decode_st_sequential(adf_data, data_offset)
        else:
            screen = decompress_rle(adf_data, data_offset)
        pixels = pixels_from_screen(screen, 0, 0, 0, 20, 200)
        filename = os.path.join(screens_dir, f'{name}.png')
        write_png_opaque(filename, pixels, palette, 320, 200)
        print(f"  {name}: 320x200 -> screens/{name}.png")

    # -----------------------------------------------------------------------
    # Also dump the pre-baked screen1.space as a full image for reference
    # -----------------------------------------------------------------------
    print("\n--- Pre-baked screen ---")
    pixels = pixels_from_screen(adf_data, SCREEN_BITMAP_OFFSET, 0, 0, 20, 200)
    filename = os.path.join(screens_dir, 'screen1-prebaked.png')
    write_png_opaque(filename, pixels, car_palette, 320, 200)
    print(f"  screen1-prebaked: 320x200 -> screens/screen1-prebaked.png")

    print(f"\nDone! All images saved to {output_dir}/")


if __name__ == '__main__':
    main()
