#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "serverthread.h"
#include "xcbutil.h"
#include "mouse.h"
#include "enforcer.h"

using boost::asio::ip::tcp;
using namespace std;

// Perform all necessary initializations
void initialize ( ) {
    xcbInit ( );
}

// Cleanup at the end of the program
void cleanup ( ) {
    xcbDestroy ( );
}

// Send the resolution of the wall to the user over a socket
void sendWindowDim(tcp::socket & sock){
    pair<int,int> dim = getResolution();					// Get the resolution
    
    std::vector<uint16_t> vec(2, 0);
    vec[0] = htons(dim.first); 								// x of resolution
    vec[1] = htons(dim.second); 							// y of resolution
    
    int num = sock.send ( boost::asio::buffer(vec) );		// Send resolution to user
}

// Start the wall and all necessary components
int main ( int argc, char * argv[] ) {
    boost::thread_group serverThreads;
    initialize ( );
    try {
    	boost::asio::io_service io_service;
    	MouseEvent event;
    	
    	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), MC_PORT));
    	
		// Loop forever waiting for new connections
    	while ( 1 ) {
			// Create a new socket
    	    tcp::socket * pSocket = new tcp::socket (io_service);
			// Accept the connection
    	    acceptor.accept(*pSocket); 
    	    cout << "Accepted remote mouse connection" << endl;
			// Give the connection its own thread
    	    serverThreads.create_thread (ServerThread(*pSocket));
    	}
    	serverThreads.join_all ( );
    }
    catch (std::exception& exc)
    {
    	cerr << exc.what() << endl;
    }
    
    cleanup ( );										// Cleanup connection
    return 0;
}

