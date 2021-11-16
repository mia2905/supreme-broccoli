#ifndef SB_APPLICATION_H
#define SB_APPLICATION_H

#include <stdio.h>

#define Assert(expression) if(!(expression)) { int* a = 0; *a = 0; } // write to a null address to crash the program deliberately
#define Print(text) { printf("%s",text); fflush(stdout); }

#define PI 3.14159265359
#define PI_2 PI/2
#define PI2  PI * 2.0

#define KiloBytes(x) (x * 1024)
#define MegaBytes(x) (KiloBytes(x) * 1024)
#define GigaBytes(x) (MegaBytes(x) * 1024)

#define ConsoleColorRed "\033[0;31m"
#define ConsoleColorDefault "\033[0m"
#define PrintNumber(label, number) printf( "%s: %f\n", label, number); fflush(stdout);
#define PrintVector(label, vector) printf( "%s: v2(%f | %f)\n", label, vector.x, vector.y); fflush(stdout);
#define PrintError(label) printf(ConsoleColorRed); printf("%s\n", label); printf(ConsoleColorDefault); fflush(stdout);
#define PrintTile(x,y,a,b) printf("tile [%d|%d] -> area [%d|%d]\n", x, y, a, b); fflush(stdout);

#define PushStruct( pool, struct ) (struct*)PushStruct_( pool, sizeof(struct) )
#define PushArray( pool, count, struct ) (struct*)PushStruct_( pool, (count)*sizeof(struct) )
#define PushBytes( pool, bytes ) (u8*)PushStruct_( pool, bytes )

#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT   600
#define TILE_SIZE        40
#define TILES_PER_AREA_Y        WINDOW_HEIGHT / TILE_SIZE
#define TILES_PER_AREA_X        WINDOW_WIDTH / TILE_SIZE

#define NR_OF_TILEAREAS 100

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

struct SoundBuffer
{
    u32 numberOfSamples;
    u32 sampleRate;
    u32 numberOfChannels;

    void* buffer;
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

#include "SB_Tilemap.h"

struct Player
{
    GeneralizedPosition playerPos;
    v2     size;
    v2     velocityVector;
    f32    acceleration; // meters per second squared -> m/s^2
    Color  color;
    Image* playerImg;
};

struct MemoryPool
{
    memory_index size;
    u8*          base;
    memory_index usedBytes;
};

struct PlatformServices
{
    Image* (*loadImage) (MemoryPool*, const char*); // image loading service
    File*  (*loadFile)  (MemoryPool*, const char*);  // file loading service
};

#include "SB_Sound.h"

struct ApplicationState
{
    bool             loading;
    Player           player;
    MemoryPool       tileMemory;
    TileMap          tilemap;
    MemoryPool       imageMemory;
    MemoryPool       mp3Memory;
    PlatformServices services;
    File*            backgroundMp3;
    Mp3Buffer*       mp3Samples;
};

void* PushStruct_( MemoryPool* pool, memory_index size )
{
    Assert( pool->usedBytes + size <= pool->size );
    u8* allocatedMemory = pool->base + pool->usedBytes;

    pool->usedBytes += size;

    return allocatedMemory;
}

/******************************************************
 * SERVICES THE PLATFORM PROVIDES TO THE APPLICATION
 ******************************************************/
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



#endif//SB_APPLICATION_H
