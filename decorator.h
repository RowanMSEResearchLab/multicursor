#ifndef DECORATOR_H
#define DECORATOR_H

#include <map>
#include "xcbutil.h"

using namespace std;


class Decorator {
	private:
		Decorator ( );
		std::map<uint32_t, int> owners;

	public:
		static Decorator * createDecorator( );
		bool isOwner ( uint32_t winId, int mouseId );
};

#endif
