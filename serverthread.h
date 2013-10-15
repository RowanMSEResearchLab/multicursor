#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <boost/asio.hpp>
#include "mouse.h"
#include "vcursor.h"
#include "enforcer.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

using namespace boost::interprocess;
using boost::asio::ip::tcp;

class ServerThread {
    
public:
    
    ServerThread ( tcp::socket & socket );
    
    void operator ( ) ();
    
private:
	// Initialize the thread    
	void servThreadInit(tcp::socket & sock);

	// Get the next event from the socket
    void getNextEvent ( tcp::socket & socket, MouseEvent & event );
    
    // Move the virtual mouse
    void mouseMove ( int x, int y );
    
    // Click the virtual mouse
    void mouseClick ( int x, int y, int buttonId );
    
    // Release a click on the virtual mouse
    void mouseUp ( int x, int y, int buttonId ) ;

	// Start a click on the virtual mouse
    void mouseDown ( int x, int y, int buttonId ) ;
    
    // Process one mouse event
    void processEvent ( MouseEvent & event ) ;
   
	// Socket used for communicating with the wall (server) 
    tcp::socket & rSocket;

	// The virtual cursor associated with the thread
    Vcursor * cursor;
	
	// The reference to the enforcer module that takes care of ownership
	Enforcer * enforcer;

	// Whether or not the connection should be terminated
    bool terminated;

	// The current window being dragged
	uint32_t this_win;

	// The mutex used to ensure virtual cursors are acquired properly
	static boost::interprocess::interprocess_semaphore mutex;
};

#endif
