#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NORMAL='\033[0m'

FRAMEWORKS='-framework CoreVideo -framework AppKit -framework AVFoundation -framework CoreGraphics'

main () {
    echo ""
    echo "---------------------------------"
    echo "    SUPREME BROCCOLI - APP"
    echo "---------------------------------"
    echo "-> CLEAN"

    rm -rf supreme-broccoli.dylib

    clang -dynamiclib -o supreme-broccoli.dylib code/SB_Application.cpp
    
    if [ -e supreme-broccoli.dylib ]
    then
        echo "-> BUILD APP: ${GREEN}SUCCESS${NORMAL}"
        sizeInKB=$(du -k supreme-broccoli.dylib | cut -f -1)
        echo "-> FILE SIZE: " $sizeInKB kB
    else
        echo "-> BUILD: ${RED}FAILED${NORMAL}"
    fi
}

time main
