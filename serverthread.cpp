#include "serverthread.h"
#include "xcbutil.h"
#include "enforcer.h"
#include "mouse.h"

using namespace std;

// Mutex used to ensure cursors are acquired properly (shared b/w all threads)
boost::interprocess::interprocess_semaphore ServerThread::mutex ( 1 );

// Create and initialize the thread
ServerThread::ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
    terminated = false;
	mutex.wait( );
    cursor = Vcursor::getCursor ( );					// Get a cursor
	mutex.post( );
	enforcer = Enforcer::getEnforcer ( );				// Get the enforcer
	enforcer->addForbidden( cursor->getMouseId ( ) );	// Add mouse window to forbidden
	servThreadInit ( socket );							// Initialize the thread
}

// While the connection exists, keep processing events
void ServerThread::operator ( ) () {
    MouseEvent event;
    while ( !terminated ) {
    	getNextEvent ( rSocket, event ) ;				// Get next event
    	processEvent ( event );							// Process that event
    }
	// Conn is closed; free (hide) the cursor
	cursor->hide();
	// Report to terminal that conn is closed (**Should be to a log file most likely**)
	CONN_CLOSED
}


// Gets the next event from the socket
void ServerThread::getNextEvent ( tcp::socket & socket, MouseEvent & event ) {
    boost::system::error_code error;
    vector<uint16_t> vec(sizeof(MouseEvent)/sizeof(uint16_t),0);
    
    // Read next event from socket 
    std::size_t length = 
    boost::asio::read(socket, 
    	boost::asio::buffer(vec), 
    	boost::asio::transfer_all(), 
    	error);
    
    // Convert from network byte order
    event.type = ntohs( vec[0] );
    event.mouseId = ntohs ( vec[1] );
    event.buttonId = ntohs ( vec[2] );
    event.x = ntohs ( vec[3] );
    event.y = ntohs ( vec[4] );
    
#if 0
    cout << "Received: " 
    << event.type << " " 
    << event.mouseId << " " 
    << event.buttonId << " "
    << event.x << " "
    << event.y << endl;
#endif
    
}
 
// Send the resolution of the display and mouseId of the cursor to the client
void ServerThread::servThreadInit(tcp::socket & sock){
    pair<int,int> dim = getResolution();				// Get the resolution
    
    std::vector<uint16_t> vec(3, 0);
    vec[0] = htons( dim.first ); 		        		// x of resolution
    vec[1] = htons( dim.second ); 		      			// y of resolution
  	vec[2] = htons( cursor->getMouseId() );				// mouseId of the cursor
    
    int num = sock.send ( boost::asio::buffer(vec) );
}

// Move the virtual mouse 
void ServerThread::mouseMove ( int x, int y ) {
    cursor->move ( x, y );
}

// Release a mouse click on the virtual cursor
void ServerThread::mouseUp ( int x, int y, int buttonId ) {
    cursor->up ( buttonId );
}

// Start a mouse click on the virtual cursor
void ServerThread::mouseDown ( int x, int y, int buttonId ) {
    cursor->down ( buttonId );
}

// Processes one mouse event
void ServerThread::processEvent ( MouseEvent & event ) {
	// The mouse is being moved
	if ( event.type == MC_BUTTON_MOVE ) {							
		mouseMove( event.x, event.y );
		// If the mouse is already down and the cursor owns the window being dragged
		if ( cursor->getMDown() &&  enforcer->isOwner ( this_win, event.mouseId ) ) {
			pair<int,int> mpos = cursor->getPosition();				// Get virtual mouse position
			xcbHideWindow ( this_win );								// Hide the window being dragged
			moveWindow ( this_win, mpos.first, mpos.second );		// Move the window being dragged
			xcbShowWindow ( this_win );								// Make the window being dragged visible again
		}
	// The user has terminated the connection
	} else if ( event.type == MC_TERMINATE ) {
		enforcer->clean ( event.mouseId );							// Clean all ownership entries for the virtual mouse
		terminated = true;											// Terminate the connection
		rSocket.close();
	// The mouse is releasing a click
	} else if ( event.type == MC_BUTTON_UP ) {
		if (event.buttonId == 1) 
			mouseUp (event.x, event.y, 1 );
	// The mouse is starting a click
	} else if ( event.type == MC_BUTTON_DOWN) {
		if ( cursor->getMDown() && event.buttonId == 3 ) {			// Already dragging; finish the drag
				cursor->setMDown(false);							// Reset this_win and cursor state to default values
				this_win = -1;
		}
		// If the virtual mouse owns the window being clicked process the event otherwise ignore it
		if ( enforcer->isOwner ( xcbGetWinIdByCoord( event.x, event.y ), event.mouseId ) ) { 
    		if ( event.buttonId == 3 ) {							// Right click; Start dragging the window
						cursor->setMDown(true);
						this_win = xcbGetWinIdByCoord ( event.x, event.y );
			}
			else  
				mouseDown ( event.x, event.y, 1 );					// Otherwise, just click the mouse
   		}
	}
}

