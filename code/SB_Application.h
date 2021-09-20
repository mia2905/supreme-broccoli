#ifndef SB_APPLICATION_H
#define SB_APPLICATION_H

#include <stdio.h>

#define Assert(expression) if(!(expression)) { int* a = 0; *a = 0; } // write to a null address to crash the program deliberately
#define Print(text) { printf(text); fflush(stdout); }
#define PrintNumber(label, number) printf( "%s: %f\n", label, number); fflush(stdout);
#define KiloBytes(x) (x * 1024)
#define MegaBytes(x) (KiloBytes(x) * 1024)
#define GigaBytes(x) (MegaBytes(x) * 1024)

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

#include "SB_Tilemap.h"

struct Player
{
    GeneralizedPosition playerPos;
    v2    size;
    v2    velocityVector;
    f32   acceleration; // meters per second squared -> m/s^2
    Color color;
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
};

struct ApplicationState
{
    bool             loading;
    Player           player;
    MemoryPool       tileMemory;
    TileMap          tilemap;
    MemoryPool       imageMemory;
    PlatformServices services;
    Image*           background;
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
