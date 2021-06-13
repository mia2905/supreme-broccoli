#ifndef SB_APPLICATION_H
#define SB_APPLICATION_H

#define Assert(expression) if(!expression) { int* a = nullptr; *a = 0; } // write to a null address to crash the program deliberately
#define Print(text) if(INFO.debugMode) { printf(text); fflush(stdout); }
#define KiloBytes(x) (x * 1024)
#define MegaBytes(x) (KiloBytes(x) * 1024)
#define GigaBytes(x) (MegaBytes(x) * 1024)

#define SCREEN_X   13
#define SCREEN_Y   27

typedef unsigned char       u8;
typedef signed   char       s8;
typedef unsigned short     u16;
typedef signed   short     s16;
typedef unsigned int       u32;
typedef signed   int       s32;
typedef unsigned long long u64;
typedef signed   long long s64;
typedef float              f32;
typedef double             f64;

struct RenderBuffer
{
    u32 width;
    u32 height;
    u32 pitch;
    u8* buffer;
};

struct KeyPress
{
    bool isDown;
};

struct UserInput
{
    KeyPress arrowUp;
    KeyPress arrowDown;
    KeyPress arrowLeft;
    KeyPress arrowRight;
    KeyPress esc;
};

struct ApplicationMemory
{
    bool  isInitialized;

    u64   permanentMemorySize; // in bytes
    void* permanentMemory;

    u64   transientMemorySize; // in bytes
    void* transientMemory;
};

struct PlatformInfo
{
    f32  deltaTimeS;
    bool debugMode;
    bool reload;
};

struct Pixel
{
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct Color
{
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
};

#include "SB_Tilemap.h"

struct Player
{
    GeneralizedPosition playerPos;
    f32   width;
    f32   height;
    f32   speed; // meters per second;
    Color color;
};

struct Screen 
{
    u32 tilesInX;
    u32 tilesInY;
};

struct ApplicationState
{
    Player  player;
    Screen  screen;
    TileMap tilemap;
};

/******************************************************
 * SERVICES THE APPLICATION PROVIDES TO THE PLATFORM
 ******************************************************/
extern "C" {
   void UpdateAndRender( ApplicationMemory* memory, 
                         RenderBuffer*      buffer, 
                         UserInput*         input,
                         PlatformInfo*      info );
}

#endif//SB_APPLICATION_H
