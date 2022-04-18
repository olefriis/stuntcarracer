#! /bin/bash

set -ex

if [ $1 == "production" ]; then
	shell_file_parameter="--shell-file custom_shell.html"
fi

emcc \
 StuntCarRacer.cpp \
 dxstdafx.cpp \
 Car\ Behaviour.cpp \
 3D\ Engine.cpp \
 Opponent\ Behaviour.cpp \
 Car.cpp \
 Track.cpp \
 wavefunctions.cpp \
 Backdrop.cpp \
 Substitutes/*.cpp \
 -ferror-limit=1000 \
 -s LLD_REPORT_UNDEFINED \
 -lopenal \
 --use-preload-plugins \
 -s USE_SDL_IMAGE=2 \
 -s SDL2_IMAGE_FORMATS='["bmp"]' \
 -s MAX_WEBGL_VERSION=2 \
 -o source.html \
 $shell_file_parameter \
 --embed-file Tracks \
 --embed-file Bitmap \
 --embed-file Sounds

echo 'Build succeeded. Now serving the result on http://localhost:8000/source.html'

python3 -m http.server
