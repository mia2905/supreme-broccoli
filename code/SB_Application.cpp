#include "SB_Application.h"



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
            p->red   = 0;
            p->green = 255;
            p->blue  = 255;
            p->alpha = 255;
            ++p;
        }
    }
}