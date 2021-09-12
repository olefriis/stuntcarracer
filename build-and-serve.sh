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
 -o source.html \
 $shell_file_parameter \
 --embed-file Tracks

echo 'Build succeeded. Now serving the result on http://localhost:8000/source.html'

python -m SimpleHTTPServer
