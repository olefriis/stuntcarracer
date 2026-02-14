#! /bin/bash

set -ex

if [ "$1" == "production" ]; then
	make production
else
	make
fi

echo 'Build succeeded. Now serving on http://localhost:8000/source.html'

cd dist && python3 -m http.server
