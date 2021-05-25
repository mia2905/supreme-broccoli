#include "SB_Application.h"
#include <stdio.h>

void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input )
{
    ApplicationState* state = (ApplicationState*)memory->permanentMemory;
    if( !memory->isInitialized )
    {
        memory->isInitialized = true;
    }

    if( input->arrowUp.ended )
    {
        state->red++;
    }

    Pixel* p = (Pixel*)buffer->buffer;

    for( u32 row=0; row<buffer->height; ++row )
    {
        for( u32 col=0; col<buffer->width; ++col )
        {
            p->red   = state->red;
            p->green = 0;
            p->blue  = 100;
            p->alpha = 255;
            ++p;
        }
    }
}