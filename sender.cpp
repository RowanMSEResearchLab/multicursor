//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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

float xscale=0, yscale=0;

// Perform all necessary initializations
void initialize ( ) {
    xcbInit ( );
    // serverInit () ;
    
}

void initNetwork ( char * ip ) {
    boost::asio::io_service io_service;
    
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(ip, "daytime");
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
    // return socket;
    
    
}

MouseEvent event;

void send ( tcp::socket & sock) {
    
    cout << "Sending : " << event.type << " " << event.mouseId << " " 
    << event.buttonId << " " << event.x << " " << event.y << endl;
    
    std::vector<uint16_t> vec(sizeof(MouseEvent)/sizeof(uint16_t),0);
    
    vec[0] = htons(event.type);
    vec[1] = htons(event.mouseId);
    vec[2] = htons(event.buttonId);
    vec[3] = htons(event.x);
    vec[4] = htons(event.y);
    
    
    //boost::asio::const_buffers_1 mybuf(vec); // = boost::asio::buffer buffer;
    
    
    // asio::async_write(socket,buffer((char*)&net.front(),6),callback);
    /* boost::system::error_code error;
    
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket sock (io_service); */
    int num = sock.send ( boost::asio::buffer(vec) );
    cout << "Sent " << num << " bytes" << endl;
}

void sendMouseMove (tcp::socket & sock, int x, int y ) {
    
    event.type = MC_BUTTON_MOVE;
    event.mouseId = 1;
    event.buttonId = 1;
    event.x = x * xscale;
    event.y = y * yscale;
    send (sock );
    
}

void sendMouseDown (tcp::socket & sock, int x, int y, int detail) {
    
    event.type = MC_BUTTON_DOWN;
    event.mouseId = 1;
    event.buttonId = detail;
    event.x = x;
    event.y = y;
    send ( sock );
}

void sendMouseUp (tcp::socket & sock, int x, int y, int detail) {
    
    event.type = MC_BUTTON_UP;
    event.mouseId = 1;
    event.buttonId = detail;
    event.x = x;
    event.y = y;
    send ( sock );    
    
}

void getWindowDim( tcp::socket & socket, int * dim){
	printf("in getwindowdim");
	boost::system::error_code error;
	vector<uint16_t> vec(2, 0);

	printf("are");
	std::size_t length = boost::asio::read(socket, boost::asio::buffer(vec), boost::asio::transfer_all(), error);

	printf("we");
	dim[0] = ntohs( vec[0] );
	dim[1] = ntohs ( vec[1] );

	printf("here");

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
	printf("stuff");

	cout << "dim: " << dim[0] << " " << dim[1] << endl;
	
	pair<int,int> dimNative = getResolution();
	xscale = dim[0] / (float) dimNative.first;
	yscale = dim[1] / (float) dimNative.second;

    	
	xcb_generic_event_t * event;
	
	// Event loop
	// Wait for button event. Print mouse position on button release and print ouch on button pres
	
	while (event = xcb_wait_for_event (display)) {
	    switch (event->response_type & ~0x80) {
	    	
	    case XCB_BUTTON_PRESS:
	    	// printf ("Ouch\n");  // Illustration of different processing for different events
	    	xcb_button_press_event_t *press;
	    	
	    	press = (xcb_button_press_event_t *) event;
	    	sendMouseDown (socket, press->event_x, press->event_y, press->detail );
	    	break;
	    	
	    case XCB_BUTTON_RELEASE: 
	    	xcb_button_press_event_t *release;
	    	
	    	release = (xcb_button_press_event_t *) event;
	    	sendMouseUp (socket, release->event_x, release->event_y, release->detail );
	    	// getMouseLocation (&windowId, &lx, &ly);
	    	// printf ("Inside window %d at (%d, %d)\n", windowId, lx, ly);
	    	break;
	    	
	    case XCB_MOTION_NOTIFY:
	    	xcb_motion_notify_event_t *motion;
	    	motion = (xcb_motion_notify_event_t *)event;
	    	//cout << "Motion " << motion->event_x << " " 
	    	//<< motion->event_y <<endl;
	    	
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


