#! /bin/bash

set -ex

./build.sh $1

echo 'Build succeeded. Now serving the result on http://localhost:8000/source.html'

python3 -m http.server
