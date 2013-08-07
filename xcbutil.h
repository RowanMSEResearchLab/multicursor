#ifndef XCBUTIL_H
#define XCBUTIL_H

#include <xcb/xcb.h>
#include <xcb/xtest.h>
#include <X11/cursorfont.h>

// Initialize the xcb connection and xcb globals
void xcbInit ( ) ;

void xcbDestroy  ( ) ;

// xcb function that clicks button at the current location
void xcbClick ( int buttonId );

// xcb function that moves the cursor to the given location
void xcbMove ( int x, int y );

// Creates specified cursor
xcb_cursor_t createCursor (uint16_t glyph);

// Gain full control of mouse
// Receive notifications for button press, button release 
// and mouse motion
void grabMouse ( ) ;

void moveWindow ( int id, int x, int y );

extern xcb_connection_t * display; 
extern int theRoot; // Root window

#endif
