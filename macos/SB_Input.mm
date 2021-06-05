#include "../code/SB_Application.h"

// MacOS key codes
#define ARROW_UP    126
#define ARROW_DOWN  125
#define ARROW_LEFT  123
#define ARROW_RIGHT 124

bool updateInput( UserInput* input, NSEvent* event )
{
    bool eventHandeled = false;
    if( [event type] == NSEventTypeKeyDown )
    {
        switch( event.keyCode )
        {
            case ARROW_UP:     
                input->arrowUp.isDown = true;
                eventHandeled = true;
                break;
            case ARROW_DOWN:  
                input->arrowDown.isDown = true;
                eventHandeled = true;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.isDown = true;
                eventHandeled = true;
                break;
            case ARROW_RIGHT: 
                input->arrowRight.isDown = true;
                eventHandeled = true;
                break;
            default: break;
        }
    }

    if( [event type] == NSEventTypeKeyUp )
    {
        switch( event.keyCode )
        {
            case ARROW_UP:    
                input->arrowUp.isDown = false;
                eventHandeled = true;
                break;
            case ARROW_DOWN:  
                input->arrowDown.isDown = false;
                eventHandeled = true;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.isDown = false; 
                eventHandeled = true;
                break;
            case ARROW_RIGHT: 
                input->arrowRight.isDown = false;
                eventHandeled = true;
                break;
            default: break;
        }
    }

    fflush( stdout );
    return eventHandeled;
}