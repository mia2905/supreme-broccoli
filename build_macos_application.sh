#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NORMAL='\033[0m'

APP_NAME='supreme-broccoli'
PLATFORM_NAME='Mac OS'
APPLICATION_CODE='./code'
CFLAGS='--debug -std=c++11 -ffast-math -lc++'
TIMEFORMAT=%R

main () {
    echo ""
    echo "---------------------------------"
    echo "    SUPREME BROCCOLI - ${PLATFORM_NAME}"
    echo "---------------------------------"
    echo "-> CLEAN"

    rm -rf ${APP_NAME}.dylib

    clang -dynamiclib -o ${APP_NAME}.dylib code/SB_Application.cpp ${CFLAGS}
    
    CODE_SIZE=$(cloc ${APPLICATION_CODE} | grep -o -E '([0-9]+)' | tail -1)

    if [ -e ${APP_NAME}.dylib ]
    then
        echo "-> BUILD APP: ${GREEN}SUCCESS${NORMAL}"
        sizeInKB=$(du -k ${APP_NAME}.dylib | cut -f -1)
        echo "-> FILE SIZE:" $sizeInKB kB
        echo "-> LINES OF CODE: " $CODE_SIZE
    else
        echo "-> BUILD APP: ${RED}FAILED${NORMAL}"
    fi
}

time main
