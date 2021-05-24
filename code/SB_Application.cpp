#include "SB_Application.h"
#include <stdio.h>


void test()
{
    int a = 6;
}

void UpdateAndRender( RenderBuffer* buffer, UserInput* input )
{
    Pixel* p = (Pixel*)buffer->buffer;

    for( u32 row=0; row<buffer->height; ++row )
    {
        for( u32 col=0; col<buffer->width; ++col )
        {
            p->red   = 100;
            p->green = 0;
            p->blue  = 100;
            p->alpha = 255;
            ++p;
        }
    }
}