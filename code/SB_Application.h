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

extern "C" {
   void Render( RenderBuffer* buffer );
}
