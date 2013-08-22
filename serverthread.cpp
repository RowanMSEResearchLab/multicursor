#include "serverthread.h"
#include "xcbutil.h"
#include "mouse.h"

using namespace std;

ServerThread::ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
	terminated = false;
    // cursor = createCursor ( XC_pirate );
    cursor = Vcursor::getCursor ( );
    // TODO: send cursor id and resolution to the client

}

void ServerThread::operator ( ) () {
    MouseEvent event;
    while ( !terminated ) {
    	getNextEvent ( rSocket, event ) ;
    	processEvent ( event );
    }
	ROAD_CLOSED
}



// Reads next event from socket
void ServerThread::getNextEvent ( tcp::socket & socket, MouseEvent & event ) {
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
void ServerThread::mouseMove ( int x, int y ) {
    
/*
    xcbMove ( x, y );
    moveWindow ( cursor, x, y );
*/
    cursor->move ( x, y );

}

// Click mouse
void ServerThread::mouseClick ( int x, int y, int buttonId ) {
    // Mouse must be moved before clicking???
    xcbMove ( x, y );
    xcbClick ( buttonId );
    
}


void ServerThread::mouseUp ( int x, int y, int buttonId ) {
    
    xcbMouseUp ( x, y, buttonId );
}

void ServerThread::mouseDown ( int x, int y, int buttonId ) {
    
    xcbMouseDown ( x, y, buttonId );
}

// Processes one mouse event
void ServerThread::processEvent ( MouseEvent & event ) {
    
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
    case MC_TERMINATE:
	terminated = true;
	rSocket.close();
	break;
    }	
    
    xcb_flush( display );
    
}



