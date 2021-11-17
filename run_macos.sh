#!/bin/sh

# start the program
./supreme-broccoli &

# watch the code folder for changes and trigger the dylib build in case the code changes
fswatch ./code | xargs -n1 -I{} ./build_macos_application.sh