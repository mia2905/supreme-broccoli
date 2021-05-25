#ifndef SB_APPLICATION_H
#define SB_APPLICATION_H

#define KiloBytes(x) (x * 1024)
#define MegaBytes(x) (KiloBytes(x) * 1024)
#define GigaBytes(x) (MegaBytes(x) * 1024)

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

struct Pixel
{
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct RenderBuffer
{
    u32 width;
    u32 height;
    u32 pitch;
    u8* buffer;
};

struct KeyPress
{
    u32  numberOfTransitions;
    bool ended;
};

struct UserInput
{
    KeyPress arrowUp;
    KeyPress arrowDown;
    KeyPress arrowLeft;
    KeyPress arrowRight;
};

struct ApplicationState
{
    u8 red;
};

struct ApplicationMemory
{
    bool  isInitialized;

    u64   permanentMemorySize; // in bytes
    void* permanentMemory;

    u64   transientMemorySize; // in bytes
    void* transientMemory;
};

extern "C" {
   void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input );
}

#endif//SB_APPLICATION_H
