#include "../code/SB_Application.h"
#include "../code/SB_Memory.cpp"

#include <AppKit/AppKit.h>
#include <mach/mach_time.h>
#include <stdio.h>
#include <dlfcn.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../tools/stb_image.h"

static bool              RUNNING       = false;
static ApplicationMemory MEMORY        = {0};
static RenderBuffer      RENDER_BUFFER = {0};
static UserInput         USER_INPUT    = {0};
static PlatformInfo      INFO          = {0};

typedef void UPDATE_AND_RENDER( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input, PlatformInfo* info );
typedef void RENDER_AUDIO( ApplicationMemory* memory, SoundBuffer* buffer );

static NSError*           ERROR         = nil;
static void*              APPLICATION   = nullptr;
static UPDATE_AND_RENDER* RENDER_FUNC   = nullptr;
static RENDER_AUDIO*      AUDIO_FUNC    = nullptr;

// platform code
#include "SB_Input.mm"
#include "SB_Audio.mm"

void UpdateAndRenderStub( ApplicationMemory* memory, 
                          RenderBuffer*      buffer, 
                          UserInput*         input,
                          PlatformInfo*      info )
{
    // no op
}

void RenderAudioStub( ApplicationMemory* memory,
                      SoundBuffer*       buffer )
{
    // no op
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
{
    @public
    NSCondition*     m_mainLoop;
    bool             m_draw;
    CVDisplayLinkRef m_displayLink;
}
@end

@implementation WindowDelegate

CVReturn update( CVDisplayLinkRef   displayLink,
                 const CVTimeStamp* inNow,
                 const CVTimeStamp* inOutputTime,
                 CVOptionFlags      flagsIn,
                 CVOptionFlags*     flagsOut,
                 void*              displayLinkContext )
{
    WindowDelegate* self = (__bridge WindowDelegate*)displayLinkContext;

    // only draw every other frame
    if( self->m_draw )
    {
        [self->m_mainLoop signal];
        self->m_draw = false;
    }
    else
    {
        self->m_draw = true;
    }
    
    return kCVReturnSuccess;
}

- (id)init
{
    if( self = [super init] )
    {
        m_mainLoop = [[NSCondition alloc] init];
        CVDisplayLinkCreateWithActiveCGDisplays( &m_displayLink );
        CVDisplayLinkSetOutputCallback( m_displayLink, update, (__bridge void *)self );
        CVDisplayLinkStart( m_displayLink );
    }

    return self;
}

- (BOOL)windowShouldClose:(NSWindow*)sender
{
    RUNNING = false;
    return YES;
}

@end

void loadApplication()
{
    NSFileManager* fileManager = [NSFileManager defaultManager];

    BOOL exists = [fileManager fileExistsAtPath:@"supreme-broccoli.dylib"];
    if( exists == YES ) 
    {
        Print( "APPLICATION EXISTS\n" );
        NSFileManager* fileManager = [NSFileManager defaultManager];

        // delete the old temp version
        [fileManager removeItemAtPath:@"supreme-broccoli-temp.dylib" 
                                error:&ERROR];

        // copy to new temp
        [fileManager copyItemAtPath:@"supreme-broccoli.dylib" 
                             toPath:@"supreme-broccoli-temp.dylib"
                              error:&ERROR];

        APPLICATION = dlopen( "supreme-broccoli-temp.dylib", RTLD_LAZY );
        RENDER_FUNC = (UPDATE_AND_RENDER*)dlsym( APPLICATION, "UpdateAndRender" );
        AUDIO_FUNC  = (RENDER_AUDIO*)dlsym( APPLICATION, "RenderAudio" );
    }     
    else
    {
        Print( "APPLICATION MISSING\n" );
        RENDER_FUNC = &UpdateAndRenderStub;
    }                    
}

void unloadApplication()
{
    if( APPLICATION != nullptr )
    {
        dlclose( APPLICATION );        
        APPLICATION = nullptr;
    }
}

int main()
{
    [NSApplication sharedApplication];

    // init an error object 
    ERROR = [NSError errorWithDomain:@"APPLICATION" code:200 userInfo:nil];
    loadApplication();

    Audio* audio = [[Audio alloc] init];
    [audio play];

    WindowDelegate* windowDelegate = [[WindowDelegate alloc] init];
    NSRect          rect           = NSMakeRect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    NSWindow*       window         = [[NSWindow alloc] initWithContentRect: rect
                                                       styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                                       backing: NSBackingStoreBuffered
                                                       defer: NO];

    NSPoint pos; 
    pos.x = [[NSScreen mainScreen] frame].size.width - [window frame].size.width ;
    pos.y = 0.0f;

    [window setFrame:CGRectMake(pos.x, pos.y, [window frame].size.width , [window frame].size.height) display:YES];    
    [window setTitle: @"SUPREME BROCCOLI"];
    [window makeKeyAndOrderFront: nil];
    [window setDelegate: windowDelegate];

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    
    u32 bytesPerPixel    = 4;    
    RENDER_BUFFER.width  = window.contentView.bounds.size.width;
    RENDER_BUFFER.height = window.contentView.bounds.size.height;
    RENDER_BUFFER.pitch  = RENDER_BUFFER.width * bytesPerPixel;
    RENDER_BUFFER.buffer = (u8*)calloc( 1, RENDER_BUFFER.pitch * RENDER_BUFFER.height );
    
    mach_timebase_info_data_t info;
    mach_timebase_info( &info );
    f64 ticksToNanoSeconds = (f64)info.numer / (f64)info.denom;
    u64 last = mach_absolute_time();

    static u32 loadCounter = 0;

    MEMORY.permanentMemorySize = MegaBytes(1024);
    MEMORY.permanentMemory = calloc( 1, MEMORY.permanentMemorySize );

    if( MEMORY.permanentMemory != nullptr )
    {
        RUNNING = true;
        ApplicationState* state   = (ApplicationState*)MEMORY.permanentMemory;
        state->services.loadImage = &LoadImage;
        state->services.loadFile  = &LoadFile;
    }

    while( RUNNING )
    {
        // this get signaled from the display link with 60FPS
        [windowDelegate->m_mainLoop wait];

        // reload the dylib every second
        if( INFO.reload )
        {
            unloadApplication();
            loadApplication();
            INFO.reload = false;
        }        

        NSEvent* event = nil;        
        do 
        {
            event = [NSApp nextEventMatchingMask: NSEventMaskAny
                                       untilDate: nil
                                          inMode: NSDefaultRunLoopMode
                                         dequeue: YES];

            // if the event is already handled here don't pass it on
            if( !updateInput( &USER_INPUT, event ) )    
            {     
                [NSApp sendEvent: event];
            }
        }
        while( event != nil );

        RENDER_FUNC( &MEMORY, &RENDER_BUFFER, &USER_INPUT, &INFO );

        @autoreleasepool {
            NSBitmapImageRep* bitmap = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes: &RENDER_BUFFER.buffer
                                                                  pixelsWide:(NSInteger)RENDER_BUFFER.width 
                                                                  pixelsHigh:(NSInteger)RENDER_BUFFER.height 
                                                                  bitsPerSample:(NSInteger)8
                                                                  samplesPerPixel:(NSInteger)4 
                                                                  hasAlpha:(BOOL)YES 
                                                                  isPlanar:(BOOL)NO 
                                                                  colorSpaceName:NSDeviceRGBColorSpace
                                                                  bytesPerRow:(NSInteger)RENDER_BUFFER.pitch
                                                                  bitsPerPixel:(NSInteger)bytesPerPixel * 8] autorelease];

            NSSize imageSize = NSMakeSize( WINDOW_WIDTH, WINDOW_HEIGHT );
            NSImage* image = [[[NSImage alloc] initWithSize: imageSize] autorelease];
            [image addRepresentation: bitmap];                                           ;

            window.contentView.layer.contents = image;
        }

        u64 endRender    = mach_absolute_time();
        f64 renderTimeNs = (f64)( endRender - last)  * ticksToNanoSeconds;
        INFO.deltaTimeS  = (f32)( renderTimeNs / (1000*1000*1000) );

        if( INFO.debugMode )
        {
            printf( "frame time [ms]: %f\n", (renderTimeNs / (1000 * 1000)));
        }        

        last = endRender;
    }

    unloadApplication();
    
    return 0;
}

/******************************************************
 * PLATFORM SERVICES
 ******************************************************/
extern "C" {
    Image* LoadImage( MemoryPool* pool, const char* filename )
    {
        Image* image = PushStruct( pool, Image );

        u8* imageData = stbi_load( filename, &image->width, &image->height, &image->channels, 4 );

        if( imageData )
        {
            // calculate the size of the memory block
            u32 memorySize = image->width * image->height * 4;
            
            // allocate memory in the pool
            image->data = PushBytes( pool, memorySize );

            // copy the image data to the pool
            memcpy( image->data, imageData, memorySize );

            // free up the memory from the loader
            stbi_image_free( imageData );
        }

        return image;
    }

    File* LoadFile( MemoryPool* pool, const char* filename )
    {
        File* file = nullptr;

        FILE* f = fopen( filename, "r" );
        if( f != nullptr )
        {
            file = PushStruct( pool, File );

            fseek( f, 0, SEEK_END );
            u32 numBytes = (u32)ftell( f );
            fseek( f, 0, SEEK_SET );

            file->data = PushBytes( pool, numBytes );
            file->size = numBytes;

            fread( file->data, numBytes, 1, f );
            fclose( f );
        }

        return file;
    }
}
