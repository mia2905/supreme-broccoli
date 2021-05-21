#include "../SB_Application.h"
#import <AppKit/AppKit.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

static bool         RUNNING       = true;
static RenderBuffer RENDER_BUFFER = {0};

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate

- (BOOL)windowShouldClose:(NSWindow*)sender
{
    RUNNING = false;
    return YES;
}

@end

int main()
{
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
    
    bool running = true;

    while( RUNNING )
    {
        Render( &RENDER_BUFFER );

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
        
        NSEvent* event = nil;
        
        do 
        {
            event = [NSApp nextEventMatchingMask: NSEventMaskAny
                                       untilDate: nil
                                          inMode: NSDefaultRunLoopMode
                                         dequeue: YES];

            switch( [event type] )
            {
                default: 
                    [NSApp sendEvent: event];
            }
        }
        while( event != nil );
    }
    
    return 0;
}
