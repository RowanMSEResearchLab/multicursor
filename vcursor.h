#ifndef VCURSOR_H
#define VCURSOR_H

#include <utility>

using namespace std;


class Vcursor {
	private:
		Vcursor(int);
		int mouseId; // The mouse id for this cursor
		int windowId; // the xcb window id for the physical cursor window
		int xpos, ypos;
		bool hidden;


	public:
		void show();
		void hide();
		void up( int buttonId );	// Mouse up
		void down( int buttonId );	// Mouse down
		pair<int,int> getPosition();
		static Vcursor * getCursor();	
		int getID ();
		void move ( int x, int y );
		bool isHidden ( );

};



#endif
