#include "serverthread.h"
#include "xcbutil.h"
#include "mouse.h"

using namespace std;

ServerThread::ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
    terminated = false;
    // cursor = createCursor ( XC_pirate );
    cursor = Vcursor::getCursor ( );
    // TODO: send cursor id and resolution to the client (init the conn)
	servThreadInit ( socket );

}

void ServerThread::operator ( ) () {
    MouseEvent event;
    while ( !terminated ) {
    	getNextEvent ( rSocket, event ) ;
    	processEvent ( event );
    }
	// Conn is closed; free the cursor
	free ( cursor );
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
 
// Send the resolution of the window
//TODO add to .h
void ServerThread::servThreadInit(tcp::socket & sock){
    pair<int,int> dim = getResolution();
    
    std::vector<uint16_t> vec(3, 0);
    vec[0] = htons( dim.first ); 		// x of resolution
    vec[1] = htons( dim.second ); 		// y of resolution
	vec[2] = htons( cursor->getMouseId() );	// mouseId of the cursor
    
    int num = sock.send ( boost::asio::buffer(vec) );
    cout << "Sent dimension ( "<< dim.first << " , " << dim.second << ") " << num << " bytes" << endl;
    
}

// Move mouse
void ServerThread::mouseMove ( int x, int y ) {
    cursor->move ( x, y );
}

// Click mouse
void ServerThread::mouseClick ( int x, int y, int buttonId ) {
    // Mouse must be moved before clicking
    cursor->move ( x, y );
    cursor->click ( buttonId );
}


void ServerThread::mouseUp ( int x, int y, int buttonId ) {
    cursor->up ( buttonId );
}

void ServerThread::mouseDown ( int x, int y, int buttonId ) {
    cursor->down ( buttonId );
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

