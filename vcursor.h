#ifndef VCURSOR_H
#define VCURSOR_H

#include <utility>

using namespace std;


class Vcursor {
	private:
		Vcursor(int);
		int id; // The mouse id for this cursor
		int windowID; // the xcb window id for the physical cursor window
		int xpos, ypos;
		bool hidden;


	public:
		void show();
		void hide();
		pair<int,int> getPosition();
		static Vcursor * getCursor();	
		int getID ();
		void move ( int x, int y );
		bool isHidden ( );

};



#endif
