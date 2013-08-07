#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xtest.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


// Convention: MC prefix for Multicursor project defns

// Mouse event types
#define MC_BUTTON_DOWN 1
#define MC_BUTTON_UP 2
#define MC_BUTTON_MOVE 3

//mask for active mouses
int ACTIVE_CURSORS = 0x00;

typedef struct _MouseEvent {
	
	int type;			// Type of mouse event
	int mouseId;		// Id of mouse that triggered event
	int buttonId;		// Id of button that was clicked
	int x, y;			// Event location
	
} MouseEvent;

// XCB globals
xcb_connection_t * display = (xcb_connection_t *) NULL;
int screen_number = 0;
xcb_setup_t *setup;
xcb_screen_iterator_t screen_iter;
int theRoot;

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

// Initialize the xcb connection and xcb globals
void xcbInit ( ) {
	
	display = xcb_connect ( NULL, &screen_number);
	if (xcb_connection_has_error(display)) {
		fprintf (stderr, "Unable to open display\n");
		
		exit (1);
	}
	setup = xcb_get_setup ( display );
	screen_iter = xcb_setup_roots_iterator(setup);
	
	int i;
	for (i = 0; i < screen_number; i++)
		xcb_screen_next(&screen_iter);
	theRoot = screen_iter.data->root;

}

void xcbDestroy  ( ) {
	xcb_disconnect ( display );
}

// xcb function that clicks button at the current location
void xcbClick ( int buttonId ) {
	
	xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
	xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
	
}

// xcb function that moves the cursor to the given location
void xcbMove ( int x, int y ) {
	
	xcb_test_fake_input ( display, XCB_MOTION_NOTIFY, 0, 0, theRoot, x, y , 0);
	
}

// Perform all necessary initializations
void initialize ( ) {
	xcbInit ( );
	// serverInit () ;
	
}

// cleanup at the end of the program
void cleanup ( ) {
	xcbDestroy ( );
	// serverShutdown ( );
}


// Get the next event
void getNextEvent ( MouseEvent * event ) {
	
	int x, y, t, id;
	
	// Get the event coordinates, id, and type
	printf ("Enter (x, y, type, id): ");
	scanf ("%d %d %d %d", &x, &y, &t, &id);

	// Build event	
	event->x = x;
	event->y = y;
	event->mouseId = id;

	// Parse event type
	//1 = mouse move
	//2 = mouse click
	switch(t)
	{
		case 1: event->type = MC_BUTTON_MOVE;
			break;
		case 2: event->type = MC_BUTTON_UP;
			break;
		default: printf("ERROR: Type must be mouse move (1) or mouse click (2).\n");
			break;
	}
}

// Move mouse
void mouseMove ( int x, int y ) {

	xcbMove ( x, y );
}

// Click mouse
void mouseClick ( int x, int y, int buttonId ) {
	// Mouse must be moved before clicking???
	xcbMove ( x, y );
	xcbClick ( buttonId );
	
}

// Processes one mouse event
void processEvent ( MouseEvent * event ) {
	
	switch ( event->type ) {
		
	case MC_BUTTON_UP:
		
		mouseClick ( event->x, event->y, 1 );
		break;
		
	case MC_BUTTON_DOWN:
		
		break;
		
	case MC_BUTTON_MOVE:
		
		mouseMove(event->x, event->y);
	}	
	
	xcb_flush( display );
	
}

int main ( int argc, char * argv[] ) {
	
	MouseEvent event;

	initialize ( );
	
	while ( 1 ) {
		getNextEvent ( & event );
		processEvent ( & event );
	}
	
	cleanup ( );
	return 0;
}
