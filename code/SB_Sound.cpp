#include "SB_Sound.h"
#include "SB_Math.h"
#include "string.h"

void RenderAudio( ApplicationMemory* memory,
                  SoundBuffer*       buffer )
{
    ApplicationState* state = (ApplicationState*)memory->permanentMemory;
  
    if( !memory->isInitialized || state->mp3Samples == nullptr )         
    {
        return;
    }

    Mp3* mp3 = state->mp3Samples;

    for( u32 c=0; c<buffer->numberOfChannels; ++c )
    {
        f32* sample    = (f32*)buffer->buffer[c];
        u32  streamPos = mp3->streamPosition + c;
        
        for( u32 i=0; i<buffer->numberOfSamples; i++ )
        {
            *sample = mp3->samples[streamPos];
            sample++;
            
            streamPos += 2;
        }
    }

    // update to the next position
    mp3->streamPosition += buffer->numberOfSamples * 2;   
}