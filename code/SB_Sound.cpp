#include "SB_Sound.h"

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