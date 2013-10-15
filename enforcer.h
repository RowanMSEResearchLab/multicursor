#ifndef ENFORCER_H
#define ENFORCER_H

#include "xcbutil.h"

using namespace std;

class Enforcer {
	private:
		// Reference to the Enforcer
		static Enforcer * enforcer;

		// The constructor to create a new Enforcer
		Enforcer ( );

	public:
		// Returns a reference to the Enforcer
		static Enforcer * getEnforcer ( );

		// Returns whether a virtual cursor owns a window
		bool isOwner ( uint32_t winId, int mouseId );

		// Adds a window to the list of forbidden (unclickable) windows
		void addForbidden ( uint32_t winId );

		// Wipes all ownership associated with a virtual cursor
		void clean ( int mouseId );
};

#endif
