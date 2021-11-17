#include "SB_Sound.h"

void RenderAudio( ApplicationMemory* memory,
                  SoundBuffer*       buffer )
{
    ApplicationState* state = (ApplicationState*)memory->permanentMemory;
  
    if( !memory->isInitialized || state->mp3Samples == NULL )         
    {
        return;
    }

    Mp3* mp3 = state->mp3Samples;

    // check the case that the output needs more samples than left in the source
    u32 samplesLeft   = (mp3->numberOfSamples - mp3->streamPosition) / 2;
    u32 samplesNeeded = buffer->numberOfSamples;
    
    for( u32 c=0; c<buffer->numberOfChannels; ++c )
    {
        f32* sample    = (f32*)buffer->buffer[c];
        u32  streamPos = mp3->streamPosition + c;
        
        for( u32 i=0; i<buffer->numberOfSamples; i++ )
        {
            if( i < samplesLeft )
            {
                *sample = mp3->samples[streamPos];
                sample++;
            }
            else
            {
                *sample = 0.0f;
                sample++;
            }
            
            streamPos += 2;
        }
    }

    if( samplesLeft < samplesNeeded )
    {
        // start over
        mp3->streamPosition = 0;
    }
    else
    {
        // update to the next position
        mp3->streamPosition += buffer->numberOfSamples * 2;     
    }
}