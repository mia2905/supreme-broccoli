#include "SB_Sound.h"

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_ONLY_MP3

#include "../tools/minimp3_ex.h"

Mp3Buffer* loadMp3Data( MemoryPool* mp3Memory, File* mp3File )
{
    mp3dec_t mp3;
    mp3dec_file_info_t info;

    if( mp3dec_load_buf( &mp3, mp3File->data, mp3File->size, &info, NULL, NULL ) )
    {
        PrintError( "ERROR: MP3 DECODING FAILED" );
    }

    Mp3Buffer* mp3Samples = PushStruct( mp3Memory, Mp3Buffer );
    mp3Samples->numberOfSamples = info.samples;
    mp3Samples->samples         = (f32*)PushBytes( mp3Memory, info.samples * sizeof(f32) );
    mp3Samples->streamPosition  = 0;

    memcpy( mp3Samples->samples, info.buffer, info.samples );

    return mp3Samples;
}

void RenderAudio( ApplicationMemory* memory,
                  SoundBuffer*       buffer )
{
    static f32 phase[2]  = {0,0};
    static f32 amplitude = 0.5;
    static f32 frequency[2] = {440.0f,800.0f};

    for( u32 c=0; c<buffer->numberOfChannels; ++c )
    {
        f32* sample = (f32*)buffer->buffer[c];

        f32  phaseIncrement = PI2 / ((f32)buffer->sampleRate / frequency[c]);
        f32  value = 0.0f;
    
        for( u32 i=0; i<buffer->numberOfSamples; i++ )
        {
            value = sinus( phase[c] ) * 0.1f;

            *sample = value;
            sample++;
            
            phase[c] += phaseIncrement;

            if( phase[c] > PI2 )  phase[c] -= PI2;
            if( phase[c] < 0.0f ) phase[c] += PI2;
        }
    }    
}