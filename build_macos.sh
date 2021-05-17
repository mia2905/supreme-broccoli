#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NORMAL='\033[0m'

FRAMEWORKS='-framework CoreVideo -framework AppKit -framework AVFoundation -framework CoreGraphics'

main () {
    echo ""
    echo "------------------------------"
    echo "build SUPREME BROCCOLI - MacOS"
    echo "------------------------------"

    clang ${FRAMEWORKS} macos/SB_Main.mm -o supreme-broccoli -O3

    if [ -e supreme-broccoli ]
    then
        echo "-> ${GREEN}SUCCESS${NORMAL}"
    else
        echo "-> ${RED}FAILED${NORMAL}"
    fi
}

time main
