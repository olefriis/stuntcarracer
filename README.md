Stunt Car Racer
===============

## Background
A fork of a fork... This is a fork of
[fluffyfreak/stuntcarracer](https://github.com/fluffyfreak/stuntcarracer), 
which is a fork of stuntcarremake from
[sourceforge](http://sourceforge.net/projects/stuntcarremake/).

Long story short, at some point the original Amiga source code for Stunt Car
Racer was released, and Daniel Vernon and Andrew Copland translated it to
C++ and made a Windows version of it.

That was not the entire original game - you could choose a track and race that
against a computer player, but it did not include the original tournament
mode or player-versus-player options. Still for fans of the original game,
this was awesome!!

Not having a Windows PC at hand, I could only play this through
something like WINE. It was not a great experience. So I set up to make a
browser version of it - modern browsers are great at 3D graphics, sound, and
input. And with the help of projects like [Emscripten](https://emscripten.org),
it's relatively easy to compile C++ into JavaScript and WebAssembly and
translate OpenGL to WebGL etc.

So I started off and implemented the Windows APIs used by the game code in a
way that Emscripten can use - the DirectX API would use OpenGL, etc.

It turns out that a similar project was already underway, which I was unaware
of: [ptitSeb/stuntcarremake](https://github.com/ptitSeb/stuntcarremake). This
project uses a separate DirectX emulation library, and as such is a much cleaner
approach. I am not worthy!

However, I'll continue this fork nevertheless! My mission is to create a
version that is more "web-native" and mobile-friendly, and which will end up
being a complete version of the original game. For example, this version includes
a tournament system like in the original game, and... two-player mode! (Really,
try this out!!)

## Try it Out!
If this is enough talk for you and you just want to play the game, go to
[https://olefriis.github.io/play](https://olefriis.github.io/play).

## Repository Structure
```
├── game-engine/       C++ source code (car physics, 3D engine, track data)
│   ├── Substitutes/   DirectX-to-OpenGL wrappers for Emscripten
│   ├── Bitmap/        Texture assets
│   ├── Tracks/        Track data files
│   └── Sounds/        Sound samples
├── web/               Browser layer
│   ├── javascript/    Game logic, service worker, multiplayer
│   ├── custom_shell.html  Emscripten HTML shell
│   ├── game.css       UI styles
│   ├── manifest.json  PWA manifest
│   └── *.png          App icons
├── signaling/         WebRTC signaling server (Ruby/Sinatra)
├── Reference only/    Original Amiga 68000 assembly source
├── Makefile           Build system
├── build-and-serve.sh Build + local dev server
└── README.md
```

## Building
Install [Emscripten](https://emscripten.org) and run

```shell
$ ./build-and-serve.sh
```

...and point a browser to
[http://localhost:8000/source.html](http://localhost:8000/source.html).

If you want to run in two-player mode, you need to run the simple Sinatra-based
web service in the `signaling` folder. Install
[Ruby](https://www.ruby-lang.org/en/), then run:

```shell
$ bundle install
$ bundle exec rackup --host 0.0.0.0 --port 8080
```

Then specify `http://localhost:8080` (or whatever local IP name your computer is
exposed as on the local network) as the Signaling Server on the Two Players join
screen.

## More on the Origin of the C++ Code
From [the original SourceForge page](https://sourceforge.net/projects/stuntcarremake/):

> About.
> This is a partial Windows conversion of the classic computer game Stunt Car Racer.  
> The game is based on the original Commodore Amiga version.
> It is written in C/C++ and utilises DirectX/Direct3D.
> 
> The game uses the original Amiga track data, sound samples and algorithms for the
> car physics (all of which were copyright Geoff Crammond / MicroStyle / MicroProse
> and now copyright the current owners which are believed to be Infogrames or
> Interactive Game Group).
