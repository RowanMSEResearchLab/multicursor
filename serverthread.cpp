#include "serverthread.h"
#include "xcbutil.h"
#include "enforcer.h"
#include "mouse.h"

using namespace std;

// Create and initialize the thread
ServerThread::ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
    terminated = false;
    // Get a cursor
    cursor = Vcursor::getCursor ( );
	enforcer = Enforcer::getEnforcer ( );
	enforcer->addForbidden( cursor->getMouseId ( ) );
	servThreadInit ( socket );
}

void ServerThread::operator ( ) () {
    MouseEvent event;
    while ( !terminated ) {
    	getNextEvent ( rSocket, event ) ;
    	processEvent ( event );
    }
	// Conn is closed; free the cursor
	cursor->hide();
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
 
// Send the resolution of the display and mouseId of the cursor to the client
void ServerThread::servThreadInit(tcp::socket & sock){
    pair<int,int> dim = getResolution();
    
    std::vector<uint16_t> vec(3, 0);
    vec[0] = htons( dim.first ); 		        // x of resolution
    vec[1] = htons( dim.second ); 		      // y of resolution
  	vec[2] = htons( cursor->getMouseId() );	// mouseId of the cursor
    
    int num = sock.send ( boost::asio::buffer(vec) );
}

// Move mouse
void ServerThread::mouseMove ( int x, int y ) {
    cursor->move ( x, y );
}

void ServerThread::mouseUp ( int x, int y, int buttonId ) {
    cursor->up ( buttonId );
}

void ServerThread::mouseDown ( int x, int y, int buttonId ) {
    cursor->down ( buttonId );
}

// Processes one mouse event
void ServerThread::processEvent ( MouseEvent & event ) {
    if ( enforcer->isOwner ( xcbGetWinIdByCoord( theRoot, event.x, event.y ), event.mouseId ) ) { 
    	switch ( event.type ) {
    	
    	case MC_BUTTON_UP:
    		// cout << "Clicking mouse at " << event.x << " " << event.y << endl;
    		mouseUp ( event.x, event.y, 1 );
    		break;
    	
    	case MC_BUTTON_DOWN:
    		mouseDown ( event.x, event.y, 1 );
    		break;
    	
    	case MC_BUTTON_MOVE:
    		mouseMove( event.x, event.y );
    		break;
		}
    //case MC_TERMINATE:
		//TODO hide and release cursor
	//	terminated = true;
	//	rSocket.close();
	//	break;
     } else if ( event.type == MC_TERMINATE ) {
		enforcer->clean ( event.mouseId );
		terminated = true;
		rSocket.close();
	}
    
    xcb_flush( display );
}

