#ifndef XCBUTIL_H
#define XCBUTIL_H

#include <xcb/xcb.h>
#include <xcb/xtest.h>
#include <X11/cursorfont.h>
#include <utility>

using std::pair;

#define MC_PORT 13
#define CONN_CLOSED printf("Remote connection disconnected.\n");

void xcbInit ( ) ; 											// Initialize the xcb connection and xcb globals
void xcbDestroy  ( ) ;										// Tear down the XCB connection
int getRoot ( );											// Get the id of the root window (desktop)
void xcbClick ( int buttonId );								// Send a click event to the current location of system cursor
void xcbMove ( int x, int y );								// Move the system cursor to the specified coords
void xcbMouseUp ( int buttonId );							// Release a mouse click
void xcbMouseDown ( int buttonId );							// Start a mouse click
xcb_cursor_t createCursor (uint16_t glyph);					// Create a new cursor glyph
void grabMouse ( ) ;										// Grab the system cursor
void moveWindow ( int id, int x, int y );					// Move a window to the specified coordinates
void xcbShowWindow ( uint32_t id );							// Make a window visible
void xcbHideWindow ( uint32_t winId );						// Make a window invisible
void xcbPullToTop ( uint32_t winId );						// Bring a window to the front of the screen (in terms of z-order)
pair<int,int> getResolution();								// Get the resolution of the screen
uint32_t xcbCreateWindow ( int );							// Create a new window
uint32_t xcbGetWinIdByCoord( int x, int y);					// Get id of window at the specified coordinates
xcb_connection_t * xcbGetDisplay ( );						// Get the XCB display connection

const uint32_t toTopVals[] = { XCB_STACK_MODE_ABOVE };		// Values for bringing a window to the front of the screen

#endif
