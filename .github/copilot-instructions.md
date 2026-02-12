# Overview

This is a fork of a really awesome project that has taken the source code from the
Amiga game Stunt Car Racer and made it work on Windows machines with DirectX. This
fork intends to make it run in a browser using the Emscripten compiler.

The changes in this fork revolve around creating a DirectX wrapper that mimics the
DirectX API but translates calls to WebGL (and SDL for sound). We want to change the
original code as little as possible, so we will implement the DirectX functions in a
way that they can be called from the original game code without modification.

The DirectX (and other) wrappers are located in the `Substitutes` folder.

# Building

To build the project, you will need to have Emscripten installed and set up on your
machine. You can follow the instructions on the Emscripten website to do this.

Then run the following command:

```bash
./build.sh
```

This will create a `source.html`, `source.js`, and `source.wasm` file in the root of the project. You can serve these files using an HTTP server, or you can just run

```bash
./build-and-serve.sh
```

...and go to http://localhost:8000/source.html. You'll see a debug log area, and
the actual game area is pretty small. To get the full game experience, instead run:

```bash
./build-and-serve.sh production
```
