

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "xcbutil.h"
#include "mouse.h"

using boost::asio::ip::tcp;
using namespace std;

xcb_cursor_t cursor;

// Perform all necessary initializations
void initialize ( ) {
    xcbInit ( );
    cursor = createCursor ( XC_pirate );
    // serverInit () ;
    
}

// cleanup at the end of the program
void cleanup ( ) {
    xcbDestroy ( );
    // serverShutdown ( );
}

class ServerThread {
private:
    tcp::socket & rSocket;
public:
    
    ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
    }
    
    void operator ( ) () {
    	MouseEvent event;
    	
    	while ( 1 ) {
    	    
    	    
    	    getNextEvent ( rSocket, event ) ;
    	    processEvent ( event );
    	    
    	}
    	
    	
    }
    
    
    
    // Reads next event from socket
    void getNextEvent ( tcp::socket & socket, MouseEvent & event ) {
    	boost::system::error_code error;
    	vector<uint16_t> vec(sizeof(MouseEvent)/sizeof(uint16_t),0);
    	
    	// Read next event from socket 
    	std::size_t length = 
    	boost::asio::read(socket, 
    	    boost::asio::buffer(vec), 
    	    boost::asio::transfer_all(), 
    	    error);
    	
    	cout << "Read : " << length << " bytes" << endl;
    	// Convert from network byte order
    	event.type = ntohs( vec[0] );
    	event.mouseId = ntohs ( vec[1] );
    	event.buttonId = ntohs ( vec[2] );
    	event.x = ntohs ( vec[3] );
    	event.y = ntohs ( vec[4] );
    	
    	cout << "Received: " 
    	<< event.type << " " 
    	<< event.mouseId << " " 
    	<< event.buttonId << " "
    	<< event.x << " "
    	<< event.y << endl;
    	
    }
    
    // Move mouse
    void mouseMove ( int x, int y ) {
    	
    	xcbMove ( x, y );
    	moveWindow ( cursor, x, y );
    }
    
    // Click mouse
    void mouseClick ( int x, int y, int buttonId ) {
    	// Mouse must be moved before clicking???
    	xcbMove ( x, y );
    	xcbClick ( buttonId );
    	
    }
    
    
    void mouseUp ( int x, int y, int buttonId ) {
    	
    	xcbMouseUp ( x, y, buttonId );
    }
    
    void mouseDown ( int x, int y, int buttonId ) {
    	
    	xcbMouseDown ( x, y, buttonId );
    }
    
    // Processes one mouse event
    void processEvent ( MouseEvent & event ) {
    	
    	switch ( event.type ) {
    	    
    	case MC_BUTTON_UP:
    	    // cout << "Clicking mouse at " << event.x << " " << event.y << endl;
    	    
    	    mouseUp ( event.x, event.y, 1 );
    	    break;
    	    
    	case MC_BUTTON_DOWN:
    	    
    	    mouseDown ( event.x, event.y, 1 );
    	    break;
    	    
    	case MC_BUTTON_MOVE:
    	    
    	    
    	    mouseMove(event.x, event.y);
    	    break;
    	}	
    	
    	xcb_flush( display );
    	
    }
    
};

int main ( int argc, char * argv[] ) {
    
    boost::thread_group serverThreads;
    
    
    
    
    initialize ( ) ;
    
    
    try {
    	
    	// Start server and accept one mouse connection
    	boost::asio::io_service io_service;
    	MouseEvent event;
    	
    	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), MC_PORT));
    	tcp::socket socket(io_service);
    	acceptor.accept(socket);
    	
    	cout << "Accepted remote mouse connection" << endl;
    	
    	serverThreads.create_thread (ServerThread(socket));
    	
    	serverThreads.join_all ( );
    }
    catch (std::exception& exc)
    {
    	cerr << exc.what() << endl;
    }
    
    cleanup ( );
    return 0;
    
    
}
