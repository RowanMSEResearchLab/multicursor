#ifndef VCURSOR_H
#define VCURSOR_H

#include <utility>
#include "xcbutil.h"

using namespace std;


class Vcursor {
	private:
		Vcursor(int, int);
		int mouseId; // The mouse id for this cursor
		uint32_t  windowId; // the xcb window id for the physical cursor window
		int xpos, ypos;
		bool hidden;


	public:
		void show();
		void hide();
		void up( int buttonId );	// Mouse up
		void down( int buttonId );	// Mouse down
		pair<int,int> getPosition();
		static Vcursor * getCursor();	
		int getMouseId ();
		xcb_window_t getWindowId ();
		void move ( int x, int y );
		bool isHidden ( );
};



#endif
