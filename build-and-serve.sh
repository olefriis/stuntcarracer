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
 -ferror-limit=1000 \
 -o source.html \
 --embed-file Tracks
python -m SimpleHTTPServer
