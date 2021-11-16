#ifndef SB_SOUND_H
#define SB_SOUND_H

#include "SB_Application.h"

struct SoundBuffer
{
    u32 numberOfSamples;
    u32 sampleRate;
    u32 numberOfChannels;

    void* buffer[2];
};

struct Mp3Buffer
{
    f32* samples;
    u32  streamPosition;
    u32  numberOfSamples;
};

struct Mp3
{
    f32* samples;
    u32  streamPosition;
    u32  numberOfSamples;
    u32  numberOfChannels;
};

#endif//SB_SOUND_H