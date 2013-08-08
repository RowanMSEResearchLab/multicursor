#include <stdio.h>


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "xcbutil.h"

// XCB globals
xcb_connection_t * display = (xcb_connection_t *) NULL;
int screen_number = 0;
xcb_setup_t *setup;
xcb_screen_iterator_t screen_iter;
int theRoot;

/*
//sets the cursorNumberth cursor to active
void setCursorActive(int cursorNumber)
{
ACTIVE_CURSORS |= 1<<cursorNumber;
}

//sets the cursorNumberth cursor to inactive
void setCursorInactive(int cursorNumber)
{
ACTIVE_CURSORS &= ~(1<<cursorNumber);
}

//sees if the cursorNumberth bit is set to active
//returns 1 if that cursor is active, 0 otherwise
int isActiveCursor(int cursorNumber)
{
return (ACTIVE_CURSORS & (1<<cursorNumber)) != 0 ? 1 : 0;
}

*/

// Initialize the xcb connection and xcb globals
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

int getRoot()
{
	if(display == NULL)
		xcbInit();
	return theRoot;
}

void xcbDestroy  ( ) {
    xcb_disconnect ( display );
}

// xcb function that clicks button at the current location
void xcbClick ( int buttonId ) {
    
    xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
    xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
    
}

void xcbMouseUp ( int x, int y, int buttonId ) {
    xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
    
}

void xcbMouseDown ( int x, int y, int buttonId ) {
    xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
    
}

// xcb function that moves the cursor to the given location
void xcbMove ( int x, int y ) {
    
    xcb_test_fake_input ( display, XCB_MOTION_NOTIFY, 0, 0, theRoot, x, y , 0);
    
}

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

void Fatal_Error ( const char * message ) {
    printf ("%s\n", message);
    exit ( 0 );
}

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

void moveWindow ( int id, int x, int y ) {
    static uint32_t values[2];
    values[0] = x;
    values[1] = y;
    
   
    xcb_configure_window (display, id, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
    
}

int* getResolution()
{
	xcb_get_geometry_cookie_t  geomCookie =
	xcb_get_geometry(display, getRoot());
	
	xcb_get_geometry_reply_t  *geom =
        xcb_get_geometry_reply (display, geomCookie, NULL);

	int* specs = new int[2];
	specs[0] = geom->width;
	specs[1] = geom->height;
	return specs;
}
