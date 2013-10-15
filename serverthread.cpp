#include "serverthread.h"
#include "xcbutil.h"
#include "enforcer.h"
#include "mouse.h"

using namespace std;

boost::interprocess::interprocess_semaphore ServerThread::mutex ( 1 );
bool m_down = false;

// Create and initialize the thread
ServerThread::ServerThread ( tcp::socket & socket ) : rSocket ( socket ) {
    terminated = false;
    // Get a cursor
	mutex.wait( );
    cursor = Vcursor::getCursor ( );
	mutex.post( );
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
    
    // cout << "Read : " << length << " bytes" << endl;
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
	if ( event.type == MC_BUTTON_MOVE ) {
		mouseMove( event.x, event.y );
		if ( cursor->getMDown() &&  enforcer->isOwner ( this_win, event.mouseId ) ) {
			pair<int,int> mpos = cursor->getPosition();
			xcbHideWindow ( this_win );
//			xcbMove( mpos.first, mpos.second );
			cout << "HERE HERE HERE HERE " <<  this << endl;
			moveWindow ( this_win, mpos.first, mpos.second );
			xcbShowWindow ( this_win );
		}
	} else if ( event.type == MC_TERMINATE ) {
		enforcer->clean ( event.mouseId );
		terminated = true;
		rSocket.close();
	} else if ( event.type == MC_BUTTON_UP ) {
		if (event.buttonId == 1) 
			mouseUp (event.x, event.y, 1 );
//		m_down = false;
//		this_win = -1;
	} else if ( event.type == MC_BUTTON_DOWN) {
		cout << "CURRENT STATE @ EVENT START: " << cursor->getMDown() << endl;
		if ( cursor->getMDown() && event.buttonId == 3 ) {	//already dragging; finish the drag
				cout << "Finishing drag for " << this << " " <<  event.mouseId << endl;
				cursor->setMDown(false);
				this_win = -1;
		}
		if ( enforcer->isOwner ( xcbGetWinIdByCoord( event.x, event.y ), event.mouseId ) ) { 
    		if ( event.buttonId == 3 ) {	//drag
		//			if ( m_down ) {			//already dragging; finish the drag
		//				cout << "Finishing drag for " << event.mouseId << endl;
		//				m_down = false;
		//				this_win = -1;
		//			} else {				//start a drag
						cout << "Starting drag for " << this << " "  << event.mouseId << endl;
						cout << "Before: " << cursor->getMDown() << endl;
						cursor->setMDown(true);
						cout << "After: " << cursor->getMDown() << endl;
						this_win = xcbGetWinIdByCoord ( event.x, event.y );
		//			}
			}
			else  
				mouseDown ( event.x, event.y, 1 );
					//if ( enforcer->isOwner ( xcbGetWinIdByCoord( event.x, event.y ), event.mouseId ) )
					//m_down = true;
					// mouseUp (event.x, event.y, 1);
					//this_win = xcbGetWinIdByCoord ( event.x, event.y );
    				// break;
   		}
	} else {
		cout << "ERRORORRORORORRORORORORORORRORORORORORROROROROEEOEEEERRRRROOOOORRRRRR" << endl;
	}
}

