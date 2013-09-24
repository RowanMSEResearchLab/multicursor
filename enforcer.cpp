
#include "enforcer.h"
#include "xcbutil.h"
#include "vcursor.h"
#include <stdio.h>
#include <map>
#include <set>

using namespace std;

typedef std::map<uint32_t, int>::iterator owners_iter;

static std::set<uint32_t> forbidden;		// List of un-ownable window id's
static std::map<uint32_t, int> owners;		// Map containing window=>owner
static bool firstTime = true;     			// First time connecting
Enforcer * Enforcer::enforcer = NULL;		// Line fixes link error. May not be needed.

Enforcer::Enforcer ( ) {
	// Add the root window (desktop) to forbidden
	forbidden.insert ( getRoot() );
}

/* Returns an instance of Enforcer */
Enforcer * Enforcer::getEnforcer ( ) {
	// If its the first time, create the new enforcer
	if ( firstTime ) {
		enforcer = new Enforcer ( );
		firstTime = false;
	}
	return enforcer;
}

/* Very that the mouse with mouseId owns the window with winId.
	Return true if the mouseId owns the window, false otherwise.
	There can only be one owner per window. */
bool Enforcer::isOwner ( uint32_t winId, int mouseId ) {
	// Check if forbidden, if so return false
	if ( forbidden.find ( winId ) != forbidden.end ( ) )
		return false;

	owners_iter itr = owners.find ( winId );
	// If window isn't in map, add it and set mouseId as owner
	if ( itr == owners.end ( ) ) {
		owners[winId] = mouseId;
		return true;
	// If window is owned by user with mouseId return true
	} else if ( itr->second == mouseId) {
		return true;
	}
	// Otherwise, return false - mouseId doesn't own window
	return false;
}

/* Add the window with window id winId to the set of forbidden windows */
void Enforcer::addForbidden ( uint32_t winId ) {
	forbidden.insert ( winId );
}

/* Remove all ownership from the mouse with mouse id mouseId.
	This should only be called when a mouse disconnects from the server */
void Enforcer::clean ( int mouseId ) {
	for ( owners_iter itr = owners.begin(); itr != owners.end(); itr++ ) {
		// If the owner is mouseId, remove the element
		// This frees the ownership on that window
		if ( itr->second == mouseId )
			printf("deleting %i entry", mouseId);
			owners.erase ( itr++ );
	}
}
