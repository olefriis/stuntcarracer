#! /bin/bash

set -ex

if [ "$1" == "debug" ]; then
	make debug
else
	make
fi

echo 'Build succeeded. Now serving on http://localhost:8000/source.html'

cd dist && python3 -m http.server
