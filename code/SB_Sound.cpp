#include "SB_Sound.h"

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