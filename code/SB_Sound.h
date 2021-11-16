#ifndef SB_SOUND_H
#define SB_SOUND_H

#include "SB_Application.h"

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_ONLY_MP3

#include "../tools/minimp3_ex.h"

struct Mp3Buffer
{
    f32* samples;
    u32  streamPosition;
    u32  numberOfSamples;
};

Mp3Buffer* loadMp3Data( MemoryPool* mp3Memory, File* mp3File );

#endif//SB_SOUND_H