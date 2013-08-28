#ifndef XCBUTIL_H
#define XCBUTIL_H

#include <xcb/xcb.h>
#include <xcb/xtest.h>
#include <X11/cursorfont.h>
#include <utility>

using std::pair;

#define MC_PORT 13
#define ROAD_CLOSED printf("THE ROAD IS CLOSED. SIGNED: THE MAYOR.\n");

// Initialize the xcb connection and xcb globals
void xcbInit ( ) ;

void xcbDestroy  ( ) ;

// xcb function that clicks button at the current location
void xcbClick ( int buttonId );

// xcb function that moves the cursor to the given location
void xcbMove ( int x, int y );

void xcbMouseUp ( int x, int y, int buttonId ) ;

void xcbMouseDown ( int x, int y, int buttonId ) ;

// Creates specified cursor
xcb_cursor_t createCursor (uint16_t glyph);

// Gain full control of mouse
// Receive notifications for button press, button release 
// and mouse motion
void grabMouse ( ) ;

void moveWindow ( int id, int x, int y );

void xcbShowWindow ( uint32_t id );

void xcbshow(xcb_window_t w);

void xcbHideWindow ( uint32_t id );


/**
	gets the resolution of the root window
*/
pair<int,int> getResolution();

uint32_t xcbCreateWindow (  int );

extern xcb_connection_t * display; 
extern int theRoot; // Root window

#endif
