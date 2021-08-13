#! /bin/bash

set -ex

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
 Substitutes/*.c \
 -ferror-limit=1000 \
 -s LEGACY_GL_EMULATION=1 \
 -o source.html \
 --embed-file Tracks

echo 'Build succeeded. Now serving the result on http://localhost:8000/source.html'

python -m SimpleHTTPServer
