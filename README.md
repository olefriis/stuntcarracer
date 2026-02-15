stuntcarracer
=============

A fork of a fork... This is a fork of
[fluffyfreak/stuntcarracer](https://github.com/fluffyfreak/stuntcarracer), 
which is a fork of stuntcarremake from
[sourceforge](http://sourceforge.net/projects/stuntcarremake/).

Long story short, at some point the original Amiga source code for Stunt Car
Racer was released, and Daniel Vernon and Andrew Copland translated it to
C++ and made a Windows version of it.

That is not the entire original game - you can choose a track and race that
against a computer player, but it does not include the original tournament
mode or player-versus-player options. Still for fans of the original game,
this was awesome!!

However, not having a Windows PC at hand, I could only play this through
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
version that is more "web-native" and mobile-friendly. And at some point I'll
also implement the original game system, i.e., tournament mode etc.

From the original SourceForge page:

> About.
> This is a partial Windows conversion of the classic computer game Stunt Car Racer.  
> The game is based on the original Commodore Amiga version.
> It is written in C/C++ and utilises DirectX/Direct3D.
> 
> The game uses the original Amiga track data, sound samples and algorithms for the car physics (all of which were copyright Geoff Crammond / MicroStyle / MicroProse and now copyright the current owners which are believed to be Infogrames or Interactive Game Group).
