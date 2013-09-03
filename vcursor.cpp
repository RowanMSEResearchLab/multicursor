
#include "vcursor.h"
#include "xcbutil.h"
#include <stdio.h>
#include <vector>

using namespace std;

static vector<Vcursor *> cursors; // Vector of all cursors
static bool firstTime = true;     // First time connecting
const int NUMCURSORS = 8;         // Number of cursors to spawn

// Different random colors to choose from
static int colors [8] = { 16777215, 858083, 0, 33023, 2263842, 14772544, 14381203, 65535 };

// Return the mouseId of the cursor
int Vcursor::getMouseId ( ) {
	return mouseId;
}

// Return the windowId of the cursor
xcb_window_t Vcursor::getWindowId ( ) {
	return windowId;
}


/* Return whether the cursor is hidden (inactive)
    or visible (active) */
bool Vcursor::isHidden ( ) {
	return hidden;
}

/* Create a new virtual cursor with the given
   mouseId and color. */
Vcursor::Vcursor ( int mouseId, int color ) {
  // Cursors start hidden and become visible when activated
	hidden = true;
	xpos = 0;
	ypos = 0;
	this->mouseId = mouseId;
	// Create the window corresponding to the vcursor
	this->windowId = xcbCreateWindow ( color );
}

/* Returns an inactive cursor if one is available
   or null if all cursors are already in use */
Vcursor * Vcursor::getCursor( ) {
	Vcursor * temp;
	int i;
  // If this is the first call, create the cursors
	if (firstTime) {
		// Create NUMCURSORS virtual cursors and place them in the vector
		for (i = 0; i < NUMCURSORS; i++)  {
			temp = new Vcursor ( i , colors[i % sizeof(colors)] ) ;
			cursors.push_back ( temp );
		}

		firstTime = false;

	}

	//TODO Semaphore acquire/release needed for sync

  // If not first time, return an inactive cursor or null
	for (i = 0; i < NUMCURSORS; i++) 
		if (cursors[i]->isHidden ( )) {
			// Unhide the cursor since it is now in use
			cursors[i]->show ( );
			return cursors[i];
		}
  // All cursor are occupied
	return NULL; 
}

// Make the cursor window visible and set hidden appropriately
void Vcursor::show( ) {
	xcbShowWindow( this->windowId ); 
	hidden = false;
  // Update the display
	xcb_flush( display );
}

// Make the cursor window invisible and set hidden appropriately
void Vcursor::hide( ) {
	xcbHideWindow( this->windowId );
	hidden = true;
  // Update the display
	xcb_flush( display );
}

// Return the current position of the cursor
pair<int,int> Vcursor::getPosition( ) {
	return pair<int,int> ( xpos, ypos );
}

// Move the cursor to the specified location
void Vcursor::move ( int x, int y ) {
	xpos = x;
	ypos = y;
	moveWindow( windowId, x, y );
}

// Mouse button is released 
void Vcursor::up ( int buttonId ) {
	xcbMouseUp ( buttonId );

  /* When a mouse click is released all other active cursor
     windows must be brought to the front of the screen */
	int i;
	for ( i = 0; i < cursors.size(); i++ ) {
		if ( !cursors[i]->isHidden( ) ) 
			xcbPullToTop ( cursors[i]->windowId );
	}
}

// Mouse button is pushed
void Vcursor::down ( int buttonId ) {
  /* Move system cursor to vcursor's current position -1 so
     the vcursor window doesn't eat the event, then click */
	xcbMove( xpos-1, ypos-1 );
	xcbMouseDown ( buttonId );
}


//VESTIGIAL FUNCTION - CAN PROBABLY BE DELETED
//void Vcursor::click ( int buttonId ) {
	//MOVE SYSTEM CURSOR TO XPOS, YPOS
	//xcbMove ( xpos, ypos );
	//xcbClick ( buttonId );
	//xcbPullToTop ( this->windowId );
//}
