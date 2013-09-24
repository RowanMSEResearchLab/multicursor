#ifndef ENFORCER_H
#define ENFORCER_H

#include "xcbutil.h"

using namespace std;

class Enforcer {
	private:
		static Enforcer * enforcer;
		Enforcer ( );

	public:
		void print ( );
		static Enforcer * getEnforcer ( );
		bool isOwner ( uint32_t winId, int mouseId );
		void addForbidden ( uint32_t winId );
		void clean ( int mouseId );
};

#endif
