#ifndef DECORATOR_H
#define DECORATOR_H

#include <map>
#include "xcbutil.h"

using namespace std;


class Decorator {
	public:
		Decorator ( );
		bool isOwner ( uint32_t winId, int mouseId );
};

#endif
