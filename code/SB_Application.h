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
    KeyPress key_f;
    KeyPress key_p;
    KeyPress key_s;
};

struct ApplicationMemory
{
    bool  isInitialized;

    u64   permanentMemorySize; // in bytes
    void* permanentMemory;
};

struct File
{
    u32 size;
    u8* data;
};

struct RenderBuffer
{
    u32 width;
    u32 height;
    u32 pitch;
    f32 metersToPixels;
    u8* buffer;
};

struct Image
{
    s32 width;
    s32 height;
    s32 channels;
    u8* data;
};

struct SoundBuffer;
struct MemoryPool;
struct Mp3Buffer;
struct TileMap;
struct Camera;
struct Player;
struct Mp3;

/******************************************************
 * SERVICES THE PLATFORM PROVIDES TO THE APPLICATION
 ******************************************************/
struct PlatformServices
{
    Image* (*loadImage)        (MemoryPool*, const char*); // image loading service
    File*  (*loadFile)         (MemoryPool*, const char*); // file loading service
    Mp3*   (*loadMp3)          (MemoryPool*, const char*); // mp3 loading service
    void   (*toggleFullScreen) (bool);                     // toggle between window and fullscreen mode  
    void   (*playAudio)        ();
    void   (*stopAudio)        ();
};

extern "C" {
    Image* LoadImage( MemoryPool* pool, const char* filename );
    File*  LoadFile(  MemoryPool* pool, const char* filename );
    Mp3*   LoadMp3(   MemoryPool* pool, const char* filename );
    void   ToggleFullScreen( bool fullscreen );
    void   PlayAudio();
    void   StopAudio();
}

/******************************************************
 * SERVICES THE APPLICATION PROVIDES TO THE PLATFORM
 ******************************************************/
extern "C" {
    void UpdateAndRender( ApplicationMemory* memory, 
                          RenderBuffer*      buffer, 
                          UserInput*         input );

    void RenderAudio(     ApplicationMemory* memory,
                          SoundBuffer*       buffer );                    
}

#include "SB_Entity.h"

struct ApplicationState
{
    MemoryPool*       appMemory;
    PlatformServices  services;
    TileMap*          tilemap;
    Camera*           camera;
    live_entities     liveEntities;
    offline_entities  offlineEntities;
    File*             backgroundMp3;
    Mp3*              mp3Samples;
    bool              loading;
    bool              fullscreen;
    f32               dt;
};

#endif//SB_APPLICATION_H
