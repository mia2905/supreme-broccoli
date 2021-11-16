#import <AVFoundation/AVFoundation.h>

@interface Audio : NSObject
{
    AVAudioEngine*               m_audioEngine;
    AVAudioNode*                 m_output;
    AVAudioFormat*               m_outputFormat;
    AVAudioFormat*               m_inputFormat;
    AVAudioMixerNode*            m_mainMixerNode;

@public
    ApplicationMemory*           m_memory;

    AVAudioSourceNode*           m_srcNode;
    AVAudioSourceNodeRenderBlock m_callback;

    u32                          m_numberOfChannels;
    u32                          m_sampleRate;
    f32                          m_amplitude;
}

- (void)play: (ApplicationMemory*)memory;

@end

@implementation Audio

- (void)printSoundSettings: (AVAudioFormat*)format
{
    switch( format.commonFormat )
    {
        case AVAudioPCMFormatFloat32: NSLog( @"sample format: float" );  break;
        case AVAudioPCMFormatFloat64: NSLog( @"sample format: double" ); break;
        case AVAudioPCMFormatInt16:   NSLog( @"sample format: short" );  break;
        case AVAudioPCMFormatInt32:   NSLog( @"sample format: int" );    break;
        default: break;
    }   

    NSLog( @"number of channels: %d", format.channelCount );
    NSLog( @"interleaved: %@", format.interleaved ? @"YES" : @"NO" );
    NSLog( @"sampleRate: %d", m_sampleRate );
}

- (id)init
{
    if( self = [super init] )
    {
        m_callback = ^OSStatus( BOOL*                 isSilence, 
                                const AudioTimeStamp* timestamp, 
                                AVAudioFrameCount     framecount, 
                                AudioBufferList*      outputData )
        {
            SoundBuffer buffer = {0};
            buffer.numberOfSamples  = framecount;
            buffer.sampleRate       = m_sampleRate;
            buffer.numberOfChannels = outputData->mNumberBuffers;

            for( u32 i=0; i<buffer.numberOfChannels; ++i )
            {
                buffer.buffer[i] = outputData->mBuffers[i].mData;
            }

            AUDIO_FUNC( m_memory, &buffer );
            
            return noErr;
        };

        m_audioEngine   = [[AVAudioEngine alloc] init];
        m_mainMixerNode = m_audioEngine.mainMixerNode;
        m_output        = m_audioEngine.outputNode;
        m_outputFormat  = [[AVAudioFormat alloc] initStandardFormatWithSampleRate: 44100.0
                                          channels: 2];
        m_inputFormat   = m_outputFormat;
        m_srcNode       = [[AVAudioSourceNode alloc] initWithRenderBlock:m_callback];                                          

        [m_audioEngine attachNode: m_srcNode];
        [m_audioEngine connect: m_srcNode       to: m_mainMixerNode format: m_inputFormat];
        [m_audioEngine connect: m_mainMixerNode to: m_output        format: m_outputFormat];

        m_numberOfChannels = m_outputFormat.channelCount;
        m_sampleRate       = m_outputFormat.sampleRate;
        m_amplitude        = 0.5f;

        m_mainMixerNode.outputVolume = m_amplitude;

        [self printSoundSettings:m_outputFormat];
    }

    return self;
}

- (void)play: (ApplicationMemory*) memory
{   
    NSError* error = [NSError alloc];

    m_memory = memory;

    if( [m_audioEngine startAndReturnError:&error] == NO )
    {
        // something went wrong while starting the engine
        NSLog( @"Couldn't start the audio" );
    }
}

@end