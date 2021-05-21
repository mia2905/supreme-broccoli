#include "../SB_Application.h"
#import <AppKit/AppKit.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

bool RUNNING = true;

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
    
    [window setBackgroundColor: NSColor.redColor];
    [window setTitle: @"SUPREME BROCCOLI"];
    [window makeKeyAndOrderFront: nil];
    [window setDelegate: windowDelegate];

    u32 imageBufferWidth  = window.contentView.bounds.size.width;
    u32 imageBufferHeight = window.contentView.bounds.size.height;
    u32 bytesPerPixel     = 4;
    u32 pitch             = imageBufferWidth * bytesPerPixel;

    u8* imageBuffer = (u8*)malloc( pitch * imageBufferHeight );

    bool running = true;

    while( RUNNING )
    {
        @autoreleasepool {
            NSBitmapImageRep* bitmap = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes: &imageBuffer
                                                                  pixelsWide:(NSInteger)imageBufferWidth 
                                                                  pixelsHigh:(NSInteger)imageBufferHeight 
                                                                  bitsPerSample:(NSInteger)8
                                                                  samplesPerPixel:(NSInteger)4 
                                                                  hasAlpha:(BOOL)YES 
                                                                  isPlanar:(BOOL)NO 
                                                                  colorSpaceName:NSDeviceRGBColorSpace
                                                                  bytesPerRow:(NSInteger)pitch
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
