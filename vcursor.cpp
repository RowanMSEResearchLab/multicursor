
#include "vcursor.h"
#include "xcbutil.h"
#include <vector>

using namespace std;


static vector<Vcursor *> cursors;
static bool firstTime = true;
const int NUMCURSORS = 8;




int Vcursor::getID () {
	return id;
}

bool Vcursor::isHidden ( ) {
	return hidden;

}


Vcursor::Vcursor (int id ) {

	hidden = true;
	xpos = 0;
	ypos = 0;
	this->id = id;
	//  Call xcbutil function to create a window
	// this will initialize the field windowID



}

Vcursor * Vcursor::getCursor() {

	Vcursor * temp;
	int i;
	if (firstTime) {
		// Create the cursor objects
		for (i = 0; i < NUMCURSORS; i++)  {
			temp = new Vcursor ( i ) ;
			cursors.push_back ( temp );
		}

		firstTime = false;

	}

	for (i = 0; i < NUMCURSORS; i++) 

		if (!cursors[i]->isHidden ( ))
			return cursors[i];



	return NULL; // All cursor are occupied

}


void Vcursor::show() {

	// TODO: Make the window appear

	hidden = false;


}


void Vcursor::hide() {

	// TODO make the window disappear possibly by moving it to 0, 0

	hidden = true;


}


pair<int,int> Vcursor::getPosition() {
	return pair<int,int> ( xpos, ypos );
}


void Vcursor::move ( int x, int y ) {
	xpos = x;
	ypos = y;

	// TODO move the window to the specified coordinates

}
