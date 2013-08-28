#include "xcbutil.h"
#include "vcursor.h" 
#include <unistd.h>
#include <stdio.h>
#include <xcb/xcb.h>
using namespace std;
int main ( ) {
	xcbInit();	//initialize xcb connection
	Vcursor * temp;
	Vcursor * temp2;
	temp = Vcursor::getCursor();
	temp2 = Vcursor::getCursor();
	temp->show();
	temp2->show();
//	xcb_unmap_window( display, temp->getWindowId() );
	//xcb_flush(display);
	sleep(5);
	temp2->hide();
	//xcb_flush(display);
	printf("%d\n", temp->getMouseId());
	printf("%d\n", temp2->getMouseId());
	xcb_flush ( display );	//send all commands before we pause so window gets shown
	pause ( );	//hold until ctrl-c
	xcb_disconnect(display);
	return 0;
}
