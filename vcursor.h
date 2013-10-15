#ifndef VCURSOR_H
#define VCURSOR_H

#include <utility>
#include "xcbutil.h"
#include <vector>

using namespace std;

class Vcursor {
	private:
		Vcursor(int, int);
		int mouseId;        // The mouse id for this cursor
		uint32_t  windowId; // The xcb windowId for the cursor window
		int xpos, ypos;     // The x and y position of the cursor
		bool hidden;        // Whether the cursor window is hidden or visible
		bool isMouseDown;   // Used for window dragging 

	public:
		void show();                  // Show the cursor window
		void hide();                  // Hide the cursor window
		void up( int buttonId );	    // Mouse up
		void down( int buttonId );	  // Mouse down
		pair<int,int> getPosition();  // Get position of the cursor
		static Vcursor * getCursor();	// Get an inactive cursor
		int getMouseId ();            // Get the cursor's mouseId
		xcb_window_t getWindowId ();  // Get the cursor's windowId
		void move ( int x, int y );   // Move cursor to specified location
		bool isHidden ( );            // Get whether cursor is hidden or visible
		void setMDown ( bool state );
		bool getMDown ( );
};

#endif
