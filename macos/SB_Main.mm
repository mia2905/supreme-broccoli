#include "../code/SB_Application.h"

#include <AppKit/AppKit.h>
#include <mach/mach_time.h>
#include <stdio.h>
#include <dlfcn.h>

// platform code
#include "SB_Input.mm"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

static bool         RUNNING       = true;
static RenderBuffer RENDER_BUFFER = {0};
static UserInput    USER_INPUT    = {0};

typedef void UPDATE_AND_RENDER( RenderBuffer* buffer, UserInput* input );

static NSError*           ERROR         = nil;
static void*              APPLICATION   = nullptr;
static UPDATE_AND_RENDER* RENDER_FUNC   = nullptr;

void UpdateAndRenderStub( RenderBuffer*, UserInput* input )
{
    // no op
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
{
    @public
    NSCondition*     m_mainLoop;
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
    [self->m_mainLoop signal];
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
        printf( "APPLICATION EXISTS\n" );
        fflush( stdout );
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
    }     
    else
    {
        printf( "APPLICATION MISSING\n" );
        fflush( stdout );

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
    // init an error object 
    ERROR = [NSError errorWithDomain:@"APPLICATION" code:200 userInfo:nil];
    loadApplication();

    WindowDelegate* windowDelegate = [[WindowDelegate alloc] init];
    NSRect          rect           = NSMakeRect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    NSWindow*       window         = [[NSWindow alloc] initWithContentRect: rect
                                                       styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                                       backing: NSBackingStoreBuffered
                                                       defer: NO];
    
    [window setTitle: @"SUPREME BROCCOLI"];
    [window makeKeyAndOrderFront: nil];
    [window setDelegate: windowDelegate];

    u32 bytesPerPixel    = 4;
    
    RENDER_BUFFER.width  = window.contentView.bounds.size.width;
    RENDER_BUFFER.height = window.contentView.bounds.size.height;
    RENDER_BUFFER.pitch  = RENDER_BUFFER.width * bytesPerPixel;
    RENDER_BUFFER.buffer = (u8*)malloc( RENDER_BUFFER.pitch * RENDER_BUFFER.height );
    
    mach_timebase_info_data_t info;
    mach_timebase_info( &info );
    f64 ticksToNanoSeconds = (f64)info.numer / (f64)info.denom;
    u64 last = mach_absolute_time();

    static u32 loadCounter = 0;
    
    while( RUNNING )
    {
        // this get signaled from the display link with 60FPS
        [windowDelegate->m_mainLoop wait];

        // reload the dylib every second
        if( ++loadCounter > 60 )
        {
            unloadApplication();
            loadApplication();
            loadCounter = 0;
        }        

        NSEvent* event = nil;
        
        do 
        {
            event = [NSApp nextEventMatchingMask: NSEventMaskAny
                                       untilDate: nil
                                          inMode: NSDefaultRunLoopMode
                                         dequeue: YES];

            updateInput( &USER_INPUT, event );
            
            [NSApp sendEvent: event];
        }
        while( event != nil );

        RENDER_FUNC( &RENDER_BUFFER, &USER_INPUT );

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

            u64 endRender    = mach_absolute_time();
            f64 renderTimeNs = (f64)(endRender - last) * ticksToNanoSeconds;

            //printf( "frame time [ms]: %f\n", (renderTimeNs / (1000 * 1000)));

            last = endRender;
        }

        resetUserInput( &USER_INPUT );
    }

    unloadApplication();
    
    return 0;
}