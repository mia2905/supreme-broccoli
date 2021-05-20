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
    NSRect    rect   = NSMakeRect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    NSWindow* window = [[NSWindow alloc] initWithContentRect: rect
                                         styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                         backing: NSBackingStoreBuffered
                                         defer: NO];
    
    [window setBackgroundColor: NSColor.redColor];
    [window setTitle: @"SUPREME BROCCOLI"];
    [window makeKeyAndOrderFront: nil];

    WindowDelegate* windowDelegate = [[WindowDelegate alloc] init];
    [window setDelegate: windowDelegate];

    bool running = true;

    while( RUNNING )
    {
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
