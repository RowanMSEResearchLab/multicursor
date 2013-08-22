#ifndef MOUSE_H
#define MOUSE_H

// Mouse event types
#define MC_INIT 1
#define MC_BUTTON_DOWN 2
#define MC_BUTTON_UP 3
#define MC_BUTTON_MOVE 4
#define MC_TERMINATE 5


typedef struct _MouseEvent {
    
    short type;			// Type of mouse event
    short mouseId;		// Id of mouse that triggered event
    short buttonId;		// Id of button that was clicked
    short x, y;			// Event location
    
    
} MouseEvent;

#endif

