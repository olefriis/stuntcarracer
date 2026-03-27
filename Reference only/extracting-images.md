# Extracting Graphics from the Original Amiga Game

First, get yourself an ADF (Amiga Disk File) of Stunt Car Racer. You can create
one yourself from the original Amiga disks using various tools, or you can search
various sites on the internet. Place it as `stunt-car-racer.adf`. Then run the
tool `extract_images.py`, and you'll get yourself a set of images from the
original Stunt Car Racer in the `extracted-images` folder.

This documents how all graphics were extracted from the original Stunt Car Racer
Amiga floppy disk image (`stunt-car-racer.adf`).

The extraction script `extract_images.py` produces 62 PNG files organized into
subfolders under `extracted-images/`.

## Background

The original game stores its cockpit/car graphics in a single pre-rendered
Amiga screen image that gets loaded into memory at boot. During initialization,
the routine at `R.69cfc` in the assembly source walks through a table of 52
graphic definitions (`graphic.info`) and extracts individual sprites from this
screen into a buffer called `cbits`.

Additionally, nine full-screen images (title, preview, etc.) are stored as
compressed or raw data, each with its own 16-color palette. Most use RLE
compression (`decrunch` routine), but `title` and `people` are stored as raw
Atari ST sequential bitplane data (4 words per 16-pixel group: one per plane).

## Key data structures in the assembly

All references are to `Reference only/StuntCarRacer.s`.

### `graphic.info` (line 27025)

A table of 52 entries, each 8 words (16 bytes). Each entry describes a graphic's
position and size within the screen image:

- Word 1: x position in 16-pixel word groups (source screen)
- Word 2: y position in lines (source screen)
- Word 3: number of 16-pixel groups wide, minus 1
- Word 4: number of lines high, minus 1
- Word 5–6: destination x/y position (used by the game, not extraction)
- Word 7–8: not used

The full table:

| Entry | Name | x groups | y | Width | Height | Notes |
|-------|------|----------|---|-------|--------|-------|
| 0–2 | Right wheels | 0,2,4 | 0 | 32px | 58 | 3 rotation frames |
| 3–5 | Left wheels | 8,10,12 | 0 | 32px | 58 | 3 rotation frames |
| 6–7 | Left flames | 0,4 | 68 | 64px | 28 | Boost flame animations |
| 8–9 | Right flames | 8,12 | 68 | 64px | 28 | Boost flame animations |
| 10 | Engine block | 2 | 123 | 256px | 21 | From car.crunched screen |
| 11–12 | Exhaust | 2,16 | 144 | 32px | 15 | Left/right, from car.crunched |
| 13–14 | Top corners | 2,17 | 16 | 16px | 6 | Left/right, from car.crunched |
| 15–16 | Chequered flag | 14 | 0,8 | 16px | 8 | Bright/dull variants |
| 17–18 | Stopwatch | 14 | 16,24 | 16px | 8 | Bright/dull variants |
| 19–20 | Left chain | 15 | 0,8 | 16px | 8 | Top/bottom |
| 21–22 | Right chain | 15 | 16,24 | 16px | 8 | Top/bottom |
| 23–24 | Hole | 16 | 0,8 | 32px | 8 | Damage overlay |
| 25–26 | Smash | 16 | 16,24 | 32px | 8 | Damage overlay |
| 27–28 | Damage bar clear | 16 | 32,40 | 32px | 8 | Two positions |
| 29–36 | Dust clouds | various | 96–134 | 48–80px | 28–38 | 8 animation frames |
| 37–48 | Sprites | various | 176 | 16px | 16 | 12 hardware sprites |
| 49 | Left flame extra | 16 | 68 | 64px | 28 | Additional flame frame |
| 50 | Right flame extra | 16 | 172 | 64px | 28 | Additional flame frame |
| 51 | Message panel | 16 | 134 | 64px | 28 | In-race message overlay |

### `car.colours` (line 3017)

The 16-color palette used during races:

```
$000 $443 $554 $770 $451 $233 $257 $247
$123 $200 $311 $422 $644 $332 $555 $777
```

These are Amiga 12-bit RGB values ($0RGB format, 4 bits per channel).

**Important:** These are *stored* values, not displayed values. The game applies
a brightness transformation before writing to hardware — see below.

### Brightness transformation (`set.amiga.colours`, line 1393)

All screen palettes pass through `set.amiga.colours` before reaching the display
hardware. This routine doubles each channel and sets its LSB if non-zero:

```
f(0) = 0
f(n) = 2n + 1   for n ≥ 1
```

This maps the stored range 0–7 to approximately the full Amiga range 0–F:

| Stored | Displayed | 8-bit |
|--------|-----------|-------|
| 0 | 0 | 0 |
| 1 | 3 | 51 |
| 2 | 5 | 85 |
| 3 | 7 | 119 |
| 4 | 9 | 153 |
| 5 | B | 187 |
| 6 | D | 221 |
| 7 | F | 255 |

For example, `$777` (the brightest car color) becomes `$FFF` = full white.

Sprite palettes (`set.sprite.colours`) bypass this transformation and use their
raw values directly — they already use the full 0–F range (e.g. `$FFF`, `$C88`).

### Screen format

The source screen is a standard Amiga 4-bitplane display:
- 320 pixels wide, 200 lines tall
- 160 bytes per scanline (word-interleaved: 4 planes × 2 bytes per 16-pixel group = 8 bytes per group, 20 groups per line)
- Each 8-byte group contains plane 0, plane 1, plane 2, plane 3 (each a 16-bit word, bit 15 = leftmost pixel)

### RLE compression (R.59450)

Nine full-screen images are RLE-compressed, each preceded by a 32-byte palette.
The decompressor works per-line, per-plane:

- 200 lines × 4 planes per line × 40 bytes per plane = 32000 bytes output
- Output is interleaved: write 2 bytes, skip 6 (for next plane's data)
- Negative command byte → RLE run (repeat next byte N+1 times)
- Positive command byte → literal copy (N+1 bytes)
- Command byte -128 is skipped

### Sprites (entries 37–48)

Sprites use the Amiga hardware sprite format (2 bitplanes) rather than the
standard 4-bitplane extraction. The `R.69e30` routine reads plane 0 (inverted)
and plane 2 from the screen. The sprite palette is:

```
$000 $000 $fff $c88
```

### Entries 10–14 (engine block, exhausts, corners)

These entries are special: they're extracted from the `car.crunched` decompressed
screen, not from the pre-baked screen image. The `R.69cfc` routine processes
entries 0–9 and 15–51 first, then decompresses `car.crunched` into `screen1.space`,
and finally extracts entries 10–14.

## The challenge: finding the data on disk

The ADF is a standard 880KB Amiga floppy image (901120 bytes). However, it does
**not** use the AmigaDOS filesystem — it's a "trackloader" disk. The bootblock
contains custom code that reads the entire game binary from raw disk sectors
directly into chip RAM. This means there's no file directory to browse; the game
code and data are laid out contiguously on disk starting from sector 0.

### Locating the screen data

Since there's no filesystem, we needed to find the screen data by searching for
known byte patterns:

1. **Found `graphic.info`** at ADF offset `0x6966C` by searching for the known
   first entry bytes.

2. **Calculated `screen1.space`** by stepping past the graphic.info table,
   graphic.pointers, and other labels, landing at offset `0x69A94`.

3. **Bitmap data** starts at `screen1.space + 34` = `0x69AB6` (34-byte header:
   2 bytes resolution + 32 bytes palette matching `car.colours`).

### Locating the crunched images

Each compressed image is preceded by its 32-byte palette. The palette patterns
were matched against known values from the assembly to find ADF offsets:

| Image | Palette offset | Data offset |
|-------|---------------|-------------|
| car | 0x1f684 | 0x1f6a4 |
| title | 0x23132 | 0x23152 |
| preview | 0x2ae74 | 0x2ae94 |
| hallfame | 0x300d0 | 0x300f0 |
| people | 0x34f76 | 0x34f96 |
| wreck | 0x3ccb8 | 0x3ccd8 |
| won | 0x43c7e | 0x43c9e |
| lost | 0x4a440 | 0x4a460 |
| promotion | 0x5034e | 0x5036e |

## Running the extraction

```bash
cd "Reference only"
python3 extract_images.py
```

This reads `stunt-car-racer.adf` and writes all images to `extracted-images/`:

### Output structure

```
extracted-images/
  wheels/          — 6 wheel rotation frames (32×58)
  flames/          — 5 boost flame animations (64×28)
  cockpit/         — Engine block (256×21), exhausts (32×15), corners (16×6)
  indicators/      — Chequered flag & stopwatch, bright/dull (16×8)
  chains/          — Left/right chain segments (16×8)
  damage/          — Holes, smashes, damage bar clears (32×8)
  dust/            — 8 dust cloud animation frames (48–80px wide)
  sprites/         — 12 hardware sprites (16×16)
  misc/            — Message panel (64×28)
  screens/         — 9 full-screen images + pre-baked screen (320×200)
```
