#ifndef SB_APPLICATION_H
#define SB_APPLICATION_H

#include <stdio.h>

#define Assert(expression) if(!(expression)) { int* a = 0; *a = 0; } // write to a null address to crash the program deliberately

#define KiloBytes(x) (x * 1024)
#define MegaBytes(x) (KiloBytes(x) * 1024)
#define GigaBytes(x) (MegaBytes(x) * 1024)

#define ConsoleColorRed "\033[0;31m"
#define ConsoleColorDefault "\033[0m"

#define Print(text) { printf("%s",text); fflush(stdout); }
#define PrintNumber(label, number) printf( "%s: %f\n", label, number); fflush(stdout);
#define PrintVector(label, vector) printf( "%s: v2(%f | %f)\n", label, vector.x, vector.y); fflush(stdout);
#define PrintError(label) printf(ConsoleColorRed); printf("%s\n", label); printf(ConsoleColorDefault); fflush(stdout);
#define PrintTile(x,y,a,b) printf("tile [%d|%d] -> area [%d|%d]\n", x, y, a, b); fflush(stdout);

#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT   600

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
typedef unsigned long      memory_index;

#include "SB_Math.h"

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
    KeyPress space;
};

struct ApplicationMemory
{
    bool  isInitialized;

    u64   permanentMemorySize; // in bytes
    void* permanentMemory;
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

struct Image
{
    s32 width;
    s32 height;
    s32 channels;
    u8* data;
};

struct File
{
    u32 size;
    u8* data;
};

struct SoundBuffer;
struct MemoryPool;
struct Mp3Buffer;
struct TileMap;
struct Player;

/******************************************************
 * SERVICES THE PLATFORM PROVIDES TO THE APPLICATION
 ******************************************************/
struct PlatformServices
{
    Image* (*loadImage) (MemoryPool*, const char*); // image loading service
    File*  (*loadFile)  (MemoryPool*, const char*); // file loading service
};

extern "C" {
    Image* LoadImage( MemoryPool* pool, const char* filename );
    File*  LoadFile(  MemoryPool* pool, const char* filename );
}

/******************************************************
 * SERVICES THE APPLICATION PROVIDES TO THE PLATFORM
 ******************************************************/
extern "C" {
   void UpdateAndRender( ApplicationMemory* memory, 
                         RenderBuffer*      buffer, 
                         UserInput*         input,
                         PlatformInfo*      info );

    void RenderAudio(    ApplicationMemory* memory,
                         SoundBuffer*       buffer );                    
}

struct ApplicationState
{
    MemoryPool*       appMemory;
    PlatformServices  services;
    TileMap*          tilemap;
    Player*           player;
    File*             backgroundMp3;
    Mp3Buffer*        mp3Samples;
    bool              loading;
};

#endif//SB_APPLICATION_H
