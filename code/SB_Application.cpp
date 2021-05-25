#include "SB_Application.h"
#include "SB_ApplicationTypes.h"

void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input )
{
    if( !memory->isInitialized )
    {
        memory->isInitialized = true;
    }

    Pixel* p = (Pixel*)buffer->buffer;

    for( u32 row=0; row<buffer->height; ++row )
    {
        for( u32 col=0; col<buffer->width; ++col )
        {
            p->red   = 0;
            p->green = 0;
            p->blue  = 100;
            p->alpha = 255;
            ++p;
        }
    }
}