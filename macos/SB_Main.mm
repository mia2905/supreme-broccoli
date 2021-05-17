#include "../SB_Application.h"
#import <AppKit/AppKit.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

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

    bool running = true;

    while( running )
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