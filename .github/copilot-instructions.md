# Overview

This is a fork of a really awesome project that has taken the source code from the
Amiga game Stunt Car Racer and made it work on Windows machines with DirectX. This
fork intends to make it run in a browser using the Emscripten compiler.

The changes in this fork revolve around creating a DirectX wrapper that mimics the
DirectX API but translates calls to WebGL (and SDL for sound). We want to change the
original code as little as possible, so we will implement the DirectX functions in a
way that they can be called from the original game code without modification.

# Building

To build the project, you will need to have Emscripten installed and set up on your
machine. You can follow the instructions on the Emscripten website to do this.

Then run the following command:

```bash
./build-and-serve.sh debug
```

...and go to http://localhost:8000/source.html. You'll see a debug log area, and
the actual game area is pretty small. To get the real game experience, instead run:

```bash
./build-and-serve.sh production
```
