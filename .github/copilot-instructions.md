# Overview

This is a fork of a really awesome project that has taken the source code from the
Amiga game Stunt Car Racer and made it work on Windows machines with DirectX. This
fork intends to make it run in a browser using the Emscripten compiler.

The changes in this fork revolve around creating a DirectX wrapper that mimics the
DirectX API but translates calls to OpenGL. Emscripten then translages OpenGL to
WebGL. In short, this project uses:
* OpenGL for graphics.
* OpenAL for audio.
* SDL for input handling.

We want to change the original code as little as possible, so we are implementing
the DirectX functions in a way that they can be called from the original game code.

The DirectX (and other) wrappers are located in the `Substitutes` folder.

# Building

To build the project, you will need to have Emscripten installed and set up on your
machine. You can follow the instructions on the Emscripten website to do this. The
project builds with Emscripten 5.0.

Then run:

```bash
make
```

This will create a `dist/` directory with everything required to run the game.You can serve the `dist/` folder with any HTTP server, or just run:

```bash
./build-and-serve.sh
```

...and go to http://localhost:8000/source.html. You'll see a debug log area, and
the actual game area is pretty small. To get the full game experience, instead run:

```bash
./build-and-serve.sh production
```

The production build uses `custom_shell.html` for a fullscreen layout and also
copies PWA assets (manifest, service worker, icons) into `dist/`.
