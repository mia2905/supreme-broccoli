#include "../code/SB_Application.h"

// MacOS key codes
#define ARROW_UP    126
#define ARROW_DOWN  125
#define ARROW_LEFT  123
#define ARROW_RIGHT 124

void resetUserInput( UserInput* input )
{
    input->arrowUp.numberOfTransitions = 0;
    input->arrowUp.ended               = false;

    input->arrowDown.numberOfTransitions = 0;
    input->arrowDown.ended               = false;

    input->arrowLeft.numberOfTransitions = 0;
    input->arrowLeft.ended               = false;

    input->arrowRight.numberOfTransitions = 0;
    input->arrowRight.ended               = false;
}

void updateInput( UserInput* input, NSEvent* event )
{
    if( [event type] == NSEventTypeKeyDown )
    {
        switch( event.keyCode )
        {
            case ARROW_UP:    
                input->arrowUp.numberOfTransitions++;    
                input->arrowUp.ended = false;
                break;
            case ARROW_DOWN:  
                input->arrowDown.numberOfTransitions++;  
                input->arrowDown.ended = false;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.numberOfTransitions++;  
                input->arrowLeft.ended = false; 
                break;
            case ARROW_RIGHT: 
                input->arrowRight.numberOfTransitions++; 
                input->arrowRight.ended = false;
                break;
            default: break;
        }
    }

    if( [event type] == NSEventTypeKeyUp )
    {
        switch( event.keyCode )
        {
            case ARROW_UP:    
                input->arrowUp.ended = true;
                break;
            case ARROW_DOWN:  
                input->arrowDown.ended = true;
                break;
            case ARROW_LEFT:  
                input->arrowLeft.ended = true; 
                break;
            case ARROW_RIGHT: 
                input->arrowRight.ended = true;
                break;
            default: break;
        }
    }
}