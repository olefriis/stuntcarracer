# Overview

The changes in the C++ code in this fork revolve around creating a DirectX wrapper
that mimics the DirectX API but translates calls to OpenGL. Emscripten then
translates OpenGL to WebGL. In short, this project uses:
* OpenGL for graphics.
* OpenAL for audio.
* SDL for input handling.

We want to change the original code as little as possible, so we are implementing
the DirectX functions in a way that they can be called from the original game code.

The DirectX (and other) wrappers are located in the `game-engine/Substitutes` folder.

There's a two-player mode that uses WebRTC for peer-to-peer communication,
and a simple Sinatra-based web service (in the `signaling` folder) for signaling.

# Repository Structure

* `game-engine/` — C++ source code, DirectX-to-OpenGL substitutes, and embedded
  assets (Bitmap, Tracks, Sounds).
* `web/` — Browser layer: HTML shell, CSS, JavaScript game logic, PWA manifest,
  icons, and service worker.
* `signaling/` — Sinatra-based WebRTC signaling server for two-player mode.
* `Reference only/` — Original Amiga 68000 assembly source for reference.
* `Makefile` — Emscripten build system.
* `build-and-serve.sh` — Build + local dev server helper.

# Building

To build the project, you will need to have Emscripten installed and set up on your
machine. You can follow the instructions on the Emscripten website to do this. The
project builds with Emscripten 5.0.

Then run:

```bash
make
```

This will create a `dist/` directory with everything required to run the game.
The default build uses `web/custom_shell.html` for a fullscreen layout and also
copies PWA assets (manifest, service worker, icons) into `dist/`.

You can serve the `dist/` folder with any HTTP server, or just run:

```bash
./build-and-serve.sh
```

...and go to http://localhost:8000/source.html.

For a debug build with the standard Emscripten shell (small canvas, log area), run:

```bash
./build-and-serve.sh debug
```

You can also specify `CHEAT=1` to enable cheat mode, which adds W/L keys to force win/loss during races:

```bash
CHEAT=1 make
```

(Remember to bust the cache after building with cheat mode, see below.)

# Cache Busting

The production build includes a service worker (`sw.js`) that caches assets for
offline/PWA use. When testing changes on a device (especially iOS simulators),
the old cached files can prevent new code from loading.

To force a fresh load, bump the cache version in `web/javascript/sw.js`:

```js
var CACHE_NAME = 'scr-v6'; // increment the number
```

Then rebuild with `make`. The new service worker will activate via
`skipWaiting()` and purge the old cache.

On iOS simulators you may also need to clear website data:
**Settings → Safari → Advanced → Website Data → Remove All Website Data**.
