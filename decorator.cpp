
#include "decorator.h"
#include "xcbutil.h"
#include "vcursor.h"
#include <stdio.h>
#include <map>
#include <vector>

using namespace std;

static vector<uint32_t> forbidden;			// List of un-ownable window id's
static std::map<uint32_t, int> owners;		// Map containing window=>owner
static bool firstTime = true;     			// First time connecting

Decorator::Decorator ( ) {
	//add all un-ownable window id's to forbidden

	// Add all cursor window id's to forbidden
	//vector<uint32_t> cursIds;
	//cursIds = Vcursor::getAllCursIds( );
	//forbidden.insert( forbidden.end(), cursIds.begin(), cursIds.end() ); 
	Vcursor::getAllCursIds( forbidden );
}

bool Decorator::isOwner ( uint32_t winId, int mouseId ) {
	//check if forbidden, if so break
	//check if in hash, if not add it
	//if in hash, compare id's return T/F accordingly
}

