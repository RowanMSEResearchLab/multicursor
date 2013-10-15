#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xtest.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "xcbutil.h"
#include "mouse.h"

using boost::asio::ip::tcp;
using namespace std;

float xscale=0, yscale=0;					// Scaling values for setting the proper resolution
int mouseId;								// The given mouse id (provided by the server)
bool terminated = false;					// Whether or not to terminate the connection

// Perform all necessary initializations
void initialize ( ) {
    xcbInit ( );
}

// Initialize the network
void initNetwork ( char * ip ) {
    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(ip, "daytime");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (!terminated && error && endpoint_iterator != end)
    {
    	socket.close();
    	socket.connect(*endpoint_iterator++, error);
    }
    if (error)
    	throw boost::system::system_error(error);
}

MouseEvent event;

// Send an event over the socket
void send ( tcp::socket & sock) {
    
    cout << "Sending : " << event.type << " " << event.mouseId << " " 
    << event.buttonId << " " << event.x << " " << event.y << endl;
    
    std::vector<uint16_t> vec(sizeof(MouseEvent)/sizeof(uint16_t),0);
    
	// Create the event and convert to network order
    vec[0] = htons(event.type);
    vec[1] = htons(event.mouseId);
    vec[2] = htons(event.buttonId);
    vec[3] = htons(event.x);
    vec[4] = htons(event.y);
	// Send the event
    int num = sock.send ( boost::asio::buffer(vec) );
	cout << "Sent " << num << " bytes" << endl;
}

// Send a mouse move event
void sendMouseMove (tcp::socket & sock, int x, int y ) {
	// Create the event
    event.type = MC_BUTTON_MOVE;
    event.mouseId = mouseId;
    event.buttonId = 1;
    event.x = x * xscale;
    event.y = y * yscale;
	// Send the event
    send ( sock );
}

// Send a mouse down event
void sendMouseDown (tcp::socket & sock, int x, int y, int detail) {
	// Create the event
    event.type = MC_BUTTON_DOWN;
    event.mouseId = mouseId;
    event.buttonId = detail;
    event.x = x;
    event.y = y;
	// Send the event
    send ( sock );
}

// Send a mouse up event
void sendMouseUp (tcp::socket & sock, int x, int y, int detail) {
	// Create the event
    event.type = MC_BUTTON_UP;
    event.mouseId = mouseId;
    event.buttonId = detail;
    event.x = x;
    event.y = y;
	// Send the event
    send ( sock );    
}

// Get the resolution of the screen
void getWindowDim( tcp::socket & socket, int * dim ){
	boost::system::error_code error;
	vector<uint16_t> vec(3, 0);

	std::size_t length = boost::asio::read(socket, boost::asio::buffer(vec), boost::asio::transfer_all(), error);

	dim[0] = ntohs ( vec[0] );					// x of resolution
	dim[1] = ntohs ( vec[1] );					// y of resolution
	mouseId = ntohs ( vec[2] );					// id of virtual cursor associated with sender
}

// Tear down the connection
void terminate(tcp::socket & sock)
{
	// Create the termination event
	event.type = MC_TERMINATE;
	event.mouseId = mouseId;
	event.buttonId = 2;
	event.x = 0;
	event.y = 0;
	// Send the termination event
	send(sock);
}

int main(int argc, char* argv[])
{
    initialize ( );
    grabMouse ( );
    try
    {
    	if (argc != 2)
    	{
    	    std::cerr << "Usage: client <host>" << endl;
    	    return 1;
    	}
    	
    	boost::asio::io_service io_service;
    	
    	tcp::resolver resolver(io_service);
    	tcp::resolver::query query(argv[1], "daytime");
    	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    	tcp::resolver::iterator end;
    	
    	tcp::socket socket(io_service);
    	boost::system::error_code error = boost::asio::error::host_not_found;
    	while (error && endpoint_iterator != end)
    	{
    	    socket.close();
    	    socket.connect(*endpoint_iterator++, error);
    	}
    	if (error)
    	    throw boost::system::system_error(error);
    	
    	cout << "Connected to the world of tomorrow!" << endl;

	int dim[2];
	getWindowDim(socket, dim);
	cout << "dim: " << dim[0] << " " << dim[1] << endl;
	pair<int,int> dimNative = getResolution();
	xscale = dim[0] / (float) dimNative.first;
	yscale = dim[1] / (float) dimNative.second;
    	
	xcb_generic_event_t * event;
	
	// Event loop
	// Wait for button event. Print mouse position on button release and print ouch on button pres
	
	while( !terminated && socket.is_open() && (event = xcb_wait_for_event (xcbGetDisplay()))) {
	    switch (event->response_type & ~0x80) {
	    	
	    case XCB_BUTTON_PRESS:
	    	xcb_button_press_event_t *press;
	    	press = (xcb_button_press_event_t *) event;
	    	sendMouseDown (socket, press->event_x, press->event_y, press->detail );
	    	break;
	    	
	    case XCB_BUTTON_RELEASE: 
	    	xcb_button_press_event_t *release;
	    	release = (xcb_button_press_event_t *) event;
	    	sendMouseUp (socket, release->event_x, release->event_y, release->detail );
			if(release->detail == 2)
			{
				terminate(socket);
				terminated = true;
				return 0;
			}
	    	break;
	    	
	    case XCB_MOTION_NOTIFY:
	    	xcb_motion_notify_event_t *motion;
	    	motion = (xcb_motion_notify_event_t *)event;
	    	sendMouseMove (socket, motion->event_x, motion->event_y );
	    	break;
	    	
	    default:
	    	break;
	    }
	}
    }
    catch (std::exception& e)
    {
    	std::cerr << e.what() << endl;
    }
    
    return 0;
}


