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
                input->arrowUp.numberOfTransitions++;    
                input->arrowUp.endedDown = true;
                eventHandeled = true;
                break;
            case ARROW_DOWN:  
                input->arrowDown.numberOfTransitions++;  
                input->arrowDown.endedDown = true;
                eventHandeled = true;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.numberOfTransitions++;  
                input->arrowLeft.endedDown = true;
                eventHandeled = true;
                break;
            case ARROW_RIGHT: 
                input->arrowRight.numberOfTransitions++; 
                input->arrowRight.endedDown = true;
                eventHandeled = true;
                break;
            default: break;
        }

        printf( "KEY DOWN\n" );
    }

    if( [event type] == NSEventTypeKeyUp )
    {
        switch( event.keyCode )
        {
            case ARROW_UP:    
                input->arrowUp.endedDown = false;
                eventHandeled = true;
                break;
            case ARROW_DOWN:  
                input->arrowDown.endedDown = false;
                eventHandeled = true;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.endedDown = false; 
                eventHandeled = true;
                break;
            case ARROW_RIGHT: 
                input->arrowRight.endedDown = false;
                eventHandeled = true;
                break;
            default: break;
        }

        printf( "KEY UP\n" );
    }

    fflush( stdout );
    return eventHandeled;
}