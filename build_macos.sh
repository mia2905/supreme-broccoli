#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NORMAL='\033[0m'

FRAMEWORKS='-framework CoreVideo -framework AppKit -framework AVFoundation -framework CoreGraphics'

main () {
    echo ""
    echo "---------------------------------"
    echo "    SUPREME BROCCOLI - MacOS"
    echo "---------------------------------"
    echo "-> CLEAN"

    rm -rf supreme-broccoli.dylib
    rm -rf supreme-broccoli

    clang -dynamiclib -o supreme-broccoli.dylib code/SB_Application.cpp --debug -std=c++11 -ffast-math
    clang ${FRAMEWORKS} macos/SB_Main.mm -o supreme-broccoli -O3 --debug -std=c++11 -ffast-math

    if [ -e supreme-broccoli.dylib ]
    then
        echo "-> BUILD APP: ${GREEN}SUCCESS${NORMAL}"
        sizeInKB=$(du -k supreme-broccoli.dylib | cut -f -1)
        echo "-> FILE SIZE: " $sizeInKB kB
    else
        echo "-> BUILD: ${RED}FAILED${NORMAL}"
    fi

    if [ -e supreme-broccoli ]
    then
        echo "-> BUILD PLATFORM: ${GREEN}SUCCESS${NORMAL}"
        sizeInKB=$(du -k supreme-broccoli | cut -f -1)
        echo "-> FILE SIZE: " $sizeInKB kB
    else
        echo "-> BUILD: ${RED}FAILED${NORMAL}"
    fi
}

time main
