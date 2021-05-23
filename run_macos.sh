#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NORMAL='\033[0m'

# start the platform
./supreme-broccoli

# watch the code folder for changes and trigger the dylib build in case the code changes