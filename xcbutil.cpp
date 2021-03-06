#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "xcbutil.h"

// XCB globals
xcb_connection_t * display = (xcb_connection_t *) NULL;			// The XCB connection to the display
int screen_number = 0;											// The screen number
xcb_setup_t *setup;												// The XCB setup
xcb_screen_iterator_t screen_iter;								// The XCB screen iterator
int theRoot;													// The root window (desktop)

// Initialize the XCB connection and xcb globals
void xcbInit ( ) {
    display = xcb_connect ( NULL, &screen_number);
    if (xcb_connection_has_error(display)) {
    	fprintf (stderr, "Unable to open display\n");
    	exit (1);
    }
    setup = (xcb_setup_t *) xcb_get_setup ( display );
    screen_iter = xcb_setup_roots_iterator(setup);
    
    int i;
    for (i = 0; i < screen_number; i++)
    	xcb_screen_next(&screen_iter);
    theRoot = screen_iter.data->root;
}

// Get the id of the root window
int getRoot()
{
    if(display == NULL)											// If the display hasnt been initialized yet
    	xcbInit();												// then initialize it
    return theRoot;
}

// Tear down the XCB connection to the display
void xcbDestroy  ( ) {
    xcb_disconnect ( display );
}

// Send a click event at the current location of the system cursor
void xcbClick ( int buttonId ) {
    xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
    xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
}

// Release a mouse click at the current location of the system cursor
void xcbMouseUp ( int buttonId ) {
    xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
   	xcb_flush ( display ); 
}

// Start a mouse click at the current location of the system cursor
void xcbMouseDown ( int buttonId ) {
    xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
   	xcb_flush ( display ); 
}

// Move the system cursor to the given coordinates
void xcbMove ( int x, int y ) {
    xcb_test_fake_input ( display, XCB_MOTION_NOTIFY, 0, 0, theRoot, x, y, 0);
	xcb_flush ( display );
}

// Brings the given window above all other open windows (in terms of z-order)
void xcbPullToTop ( uint32_t winId ) {
	xcb_configure_window ( display, winId, XCB_CONFIG_WINDOW_STACK_MODE, toTopVals );
	xcb_flush ( display );
}

// Creates a new cursor glyph
xcb_cursor_t createCursor ( uint16_t glyph)
{
    static xcb_font_t cursor_font;
    xcb_cursor_t cursor;
    
    if (!cursor_font) {
    	cursor_font = xcb_generate_id (display);
    	xcb_open_font (display, cursor_font, strlen ("cursor"), "cursor");
    }
    
    cursor = xcb_generate_id (display);
    xcb_create_glyph_cursor (display, cursor, cursor_font, cursor_font,
    	glyph, glyph + 1,
    	0, 0, 0, 0xffff, 0xffff, 0xffff);  /* rgb, rgb */
    xcb_map_window ( display, cursor );
    
    return cursor;
}

// Processes and prints errors then exits
void Fatal_Error ( const char * message ) {
    printf ("%s\n", message);
    exit ( 0 );
}

// Grabs the system cursor
void grabMouse ( ) {
    xcb_cursor_t cursor;
    cursor = createCursor ( XC_pirate );
    xcb_grab_pointer_cookie_t gpcookie;
    gpcookie = xcb_grab_pointer(
    	display,
    	0,
    	theRoot,       
    	XCB_EVENT_MASK_BUTTON_PRESS | 
    	XCB_EVENT_MASK_BUTTON_RELEASE| 
    	XCB_EVENT_MASK_POINTER_MOTION ,
    	XCB_GRAB_MODE_ASYNC, 
    	XCB_GRAB_MODE_ASYNC,
    	theRoot, 
    	cursor, 
    	XCB_TIME_CURRENT_TIME);
    
    xcb_grab_pointer_reply_t * gpreply;
    xcb_generic_error_t *err;
    
    gpreply = xcb_grab_pointer_reply (display, gpcookie, &err);
    if (gpreply->status != XCB_GRAB_STATUS_SUCCESS)
    	Fatal_Error ("Can't grab the mouse.");
}

// Moves the specified window to the specified coordinates
void moveWindow ( int id, int x, int y ) {
    uint32_t values[2];
    values[0] = x;
    values[1] = y;
    
    xcb_configure_window (display, id, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
    xcb_flush ( display );
}

// Gets the resolution of the display
pair<int,int> getResolution()
{
    xcb_get_geometry_cookie_t  geomCookie =
    xcb_get_geometry(display, getRoot());
    
    xcb_get_geometry_reply_t  *geom =
    xcb_get_geometry_reply (display, geomCookie, NULL);
    
    int* specs = new int[2];
    specs[0] = geom->width;
    specs[1] = geom->height;
    return pair<int,int>(geom->width,geom->height);
}

// Makes a window invisible
void xcbHideWindow( uint32_t windowId )
{
	xcb_unmap_window(display, windowId);
	xcb_flush ( display );	
}

// Makes a window visible
void xcbShowWindow( uint32_t windowId )
{
	xcb_map_window(display, windowId);
	xcb_flush ( display );
}

// Creates a new window with the specified color
xcb_window_t xcbCreateWindow ( int color ) {
	const xcb_setup_t *setup = xcb_get_setup ( display );
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator ( setup );
	xcb_screen_t *screen = iter.data;

	uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t values[2] = {color, 0};

	xcb_window_t window = xcb_generate_id ( display );

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[1] = XCB_EVENT_MASK_EXPOSURE;
	xcb_create_window (display,                    				/* Connection          */
                           XCB_COPY_FROM_PARENT,          		/* depth (same as root)*/
                           window,                        		/* window Id           */
                           screen->root,                  		/* parent window       */
                           0, 0,                          		/* x, y                */
                           40, 40,                      		/* width, height       */
                           10,                            		/* border_width        */
                           XCB_WINDOW_CLASS_INPUT_OUTPUT, 		/* class               */
                           screen->root_visual,           		/* visual              */
                           mask, values );                     	/* masks, not used yet */
	return window;
}

// Gets the id of the window that is currently at the specified coordinates
uint32_t xcbGetWinIdByCoord( int x, int y )
{
    xcb_query_pointer_cookie_t qpcookie;
    xcb_query_pointer_reply_t * qpreply;
	
	xcbMove ( x-1, y-1 );    
    qpcookie = xcb_query_pointer ( display, theRoot );
    qpreply = xcb_query_pointer_reply ( display, qpcookie, NULL);
    return qpreply->child;
}

// Gets the XCB display connection
xcb_connection_t * xcbGetDisplay ( ) {
	return display;
}


