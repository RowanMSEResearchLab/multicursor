#ifndef VCURSOR_H
#define VCURSOR_H

#include <utility>

using namespace std;


class Vcursor {
	private:
		Vcursor(int);
		int id;
		int windowID;
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
