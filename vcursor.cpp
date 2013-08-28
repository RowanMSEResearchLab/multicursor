
#include "vcursor.h"
#include "xcbutil.h"
#include <stdio.h>
#include <vector>

using namespace std;

static vector<Vcursor *> cursors;
static bool firstTime = true;
const int NUMCURSORS = 8;

//a list of different random colors to choose from
static int colors [8] = { 16777215, 858083, 0, 33023, 2263842, 14772544, 14381203, 65535 };

int Vcursor::getMouseId ( ) {
	return mouseId;
}

xcb_window_t Vcursor::getWindowId ( ) {
	return windowId;
}

bool Vcursor::isHidden ( ) {
	return hidden;

}

Vcursor::Vcursor ( int mouseId, int color ) {
	hidden = true;
	xpos = 0;
	ypos = 0;
	this->mouseId = mouseId;
	//  Call xcbutil function to create a window
	// this will initialize the field windowID
	this->windowId = xcbCreateWindow ( color );
}

Vcursor * Vcursor::getCursor( ) {
	Vcursor * temp;
	int i;
	if (firstTime) {
		// Create the cursor objects
		for (i = 0; i < NUMCURSORS; i++)  {
			temp = new Vcursor ( i , colors[i % sizeof(colors)] ) ;
			cursors.push_back ( temp );
		}

		firstTime = false;

	}

	//TODO Semaphore acquire/release needed for sync
	for (i = 0; i < NUMCURSORS; i++) 

		if (cursors[i]->isHidden ( )) {
			// Unhide the cursor since it is now in use!!!!
			cursors[i]->hidden = false;
			return cursors[i];
		}
	return NULL; // All cursor are occupied

}

/* NOTE: If show() or hide() don't seem to be working make sure you
	are flushing the xcb connection before pausing!!!! */
void Vcursor::show( ) {

	xcbShowWindow( this->windowId ); 
	printf("%d\n", windowId);
	hidden = false;
	xcb_flush( display );
}

void Vcursor::hide( ) {
	xcbHideWindow( this->windowId );
	printf("HIDING WINDOW: %d\n", windowId);
	hidden = true;
	xcb_flush( display );
}

pair<int,int> Vcursor::getPosition( ) {
	return pair<int,int> ( xpos, ypos );
}

void Vcursor::move ( int x, int y ) {
	xpos = x;
	ypos = y;

	// TODO Move the window to the specified coordinates
	moveWindow( windowId, x, y );

}

void Vcursor::up ( int buttonId ) {
	xcbMouseUp ( xpos, ypos, buttonId );
}

void Vcursor::down ( int buttonId ) {
	xcbMouseDown ( xpos, ypos, buttonId );
}

void Vcursor::click ( int buttonId ) {
	xcbClick ( buttonId );
}
