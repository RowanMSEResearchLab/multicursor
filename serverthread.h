#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <boost/asio.hpp>
#include "mouse.h"
#include "vcursor.h"

using boost::asio::ip::tcp;

class ServerThread {
    
public:
    
    ServerThread ( tcp::socket & socket );
    
    void operator ( ) ();
    
private:
    
	void servThreadInit(tcp::socket & sock);

    void getNextEvent ( tcp::socket & socket, MouseEvent & event );
    
    // Move mouse
    void mouseMove ( int x, int y );
    
    // Click mouse
    void mouseClick ( int x, int y, int buttonId );
    
    
    void mouseUp ( int x, int y, int buttonId ) ;
    void mouseDown ( int x, int y, int buttonId ) ;
    
    // Processes one mouse event
    void processEvent ( MouseEvent & event ) ;
    
    tcp::socket & rSocket;
    // int cursor;
    Vcursor * cursor;
    bool terminated;
};

#endif
