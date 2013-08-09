#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "serverthread.h"
#include "xcbutil.h"

using boost::asio::ip::tcp;
using namespace std;



// Perform all necessary initializations
void initialize ( ) {
    xcbInit ( );
    
    // serverInit () ;
    
}

// cleanup at the end of the program
void cleanup ( ) {
    xcbDestroy ( );
    // serverShutdown ( );
}



void sendWindowDim(tcp::socket & sock){
    pair<int,int> dim = getResolution();
    
    std::vector<uint16_t> vec(2, 0);
    vec[0] = htons(dim.first); // htons(dim[0]);
    vec[1] = htons(dim.second); // dim[1]);
    
    int num = sock.send ( boost::asio::buffer(vec) );
    cout << "Sent dimension ( "<< dim.first << " , " << dim.second << ") " << num << " bytes" << endl;
    
}

int main ( int argc, char * argv[] ) {
    
    boost::thread_group serverThreads;
    
    
    
    
    initialize ( ) ;
    
    
    try {
    	boost::asio::io_service io_service;
    	MouseEvent event;
    	
    	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), MC_PORT));
    	
    	while ( 1 ) {
    	    
    	    
    	    // tcp::socket socket(io_service);
    	    tcp::socket * pSocket = new tcp::socket (io_service);
    	    acceptor.accept(*pSocket); 
            sendWindowDim( *pSocket );
    	    
    	    cout << "Accepted remote mouse connection" << endl;
    	    
    	    serverThreads.create_thread (ServerThread(*pSocket));
    	    
    	}
    	
    	serverThreads.join_all ( );
    }
    catch (std::exception& exc)
    {
    	cerr << exc.what() << endl;
    }
    
    
    cleanup ( );
    return 0;
    
    
}
