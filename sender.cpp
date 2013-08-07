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
#include <xcb/xcb_keysyms.h>
#include <xcb/xtest.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <X11/cursorfont.h>
#include "mouse.h"

using boost::asio::ip::tcp;



// XCB globals
xcb_connection_t * display = (xcb_connection_t *) NULL;
int screen_number = 0;
xcb_setup_t *setup;
xcb_screen_iterator_t screen_iter;
int theRoot;

// Initialize the xcb connection and xcb globals
void xcbInit ( ) {
    
    display = xcb_connect ( NULL, &screen_number);
    if (xcb_connection_has_error(display)) {
    	fprintf (stderr, "Unable to open display\n");
    	
    	exit (1);
    }
    setup = (xcb_setup_t *) xcb_get_setup ( display );
    screen_iter = xcb_setup_roots_iterator(setup);
    
    int i;
    for (i = 0; i < screen_number; i++)
    	xcb_screen_next(&screen_iter);
    theRoot = screen_iter.data->root;
    
}

void xcbDestroy  ( ) {
    xcb_disconnect ( display );
}

// xcb function that clicks button at the current location
void xcbClick ( int buttonId ) {
    
    xcb_test_fake_input( display, XCB_BUTTON_PRESS, buttonId, 0, XCB_NONE, 0, 0, 0 );
    xcb_test_fake_input( display, XCB_BUTTON_RELEASE, buttonId, 0, XCB_NONE, 0, 0, 0 );
    
}

// xcb function that moves the cursor to the given location
void xcbMove ( int x, int y ) {
    
    xcb_test_fake_input ( display, XCB_MOTION_NOTIFY, 0, 0, theRoot, x, y , 0);
    
}

xcb_cursor_t
Create_Font_Cursor (xcb_connection_t *dpy, uint16_t glyph)
{
    static xcb_font_t cursor_font;
    xcb_cursor_t cursor;
    
    if (!cursor_font) {
    	cursor_font = xcb_generate_id (dpy);
    	xcb_open_font (dpy, cursor_font, strlen ("cursor"), "cursor");
    }
    
    cursor = xcb_generate_id (dpy);
    xcb_create_glyph_cursor (dpy, cursor, cursor_font, cursor_font,
    	glyph, glyph + 1,
    	0, 0, 0, 0xffff, 0xffff, 0xffff);  /* rgb, rgb */
    
    return cursor;
}

void Fatal_Error ( const char * message ) {
    printf ("%s\n", message);
    exit ( 0 );
}


void grabMouse ( ) {
    
    
    xcb_cursor_t cursor;
    cursor = Create_Font_Cursor (display, XC_crosshair);
    xcb_grab_pointer_cookie_t gpcookie;
    
    gpcookie = xcb_grab_pointer(
    	display,
    	0,
    	theRoot,       
    	XCB_EVENT_MASK_BUTTON_PRESS | 
    	XCB_EVENT_MASK_BUTTON_RELEASE| 
    	XCB_EVENT_MASK_POINTER_MOTION ,
    	XCB_GRAB_MODE_ASYNC, 
    	XCB_GRAB_MODE_ASYNC,
    	theRoot, 
    	cursor, 
    	XCB_TIME_CURRENT_TIME);
    
    xcb_grab_pointer_reply_t * gpreply;
    xcb_generic_error_t *err;
    
    gpreply = xcb_grab_pointer_reply (display, gpcookie, &err);
    if (gpreply->status != XCB_GRAB_STATUS_SUCCESS)
    	Fatal_Error ("Can't grab the mouse.");
}

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
    
    std::cout << "Sending : " << event.type << " " << event.mouseId << " " 
    << event.buttonId << " " << event.x << " " << event.y << std::endl;
    
    std::vector<uint16_t> vec(5,0);
    
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
    std::cout << "Sent " << num << " bytes" << std::endl;
}

void sendMouseMove (tcp::socket & sock, int x, int y ) {
    
    event.type = MC_BUTTON_MOVE;
    event.mouseId = 1;
    event.buttonId = 1;
    event.x = x;
    event.y = y;
    send (sock );
    
}

void sendMouseDown (tcp::socket & sock, int x, int y) {
    
    event.type = MC_BUTTON_DOWN;
    event.mouseId = 1;
    event.buttonId = 1;
    event.x = x;
    event.y = y;
    send ( sock );
}

void sendMouseUp (tcp::socket & sock, int x, int y) {
    
    event.type = MC_BUTTON_UP;
    event.mouseId = 1;
    event.buttonId = 1;
    event.x = x;
    event.y = y;
    send ( sock );    
    
}


int main(int argc, char* argv[])
{
    
    initialize ( );
    grabMouse ( );
    try
    {
    	if (argc != 2)
    	{
    	    std::cerr << "Usage: client <host>" << std::endl;
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
    
    std::cout << "Connected!" << std::endl;
    	
    	// socket = initNetwork ( argv[1] );
    	
    	//for (;;)
    	// {
    	// boost::array<char, 128> buf;
    	// boost::system::error_code error;
    	
    	// size_t len = socket->read_some(boost::asio::buffer(buf), error);
    	
    	// if (error == boost::asio::error::eof)
    	//   std::cerr << "End" << std::endl;
    	
    	
    	// break; // Connection closed cleanly by peer.
    	// else if (error)
    	//    throw boost::system::system_error(error); // Some other error.
    	
    	// std::cout.write(buf.data(), len);
    	
    	//}
    	
	xcb_generic_event_t * event;
	
	// Event loop
	// Wait for button event. Print mouse position on button release and print ouch on button pres
	
	while (event = xcb_wait_for_event (display)) {
	    switch (event->response_type & ~0x80) {
	    	
	    case XCB_BUTTON_PRESS:
	    	// printf ("Ouch\n");  // Illustration of different processing for different events
	    	xcb_button_press_event_t *press;
	    	
	    	press = (xcb_button_press_event_t *) event;
	    	sendMouseDown (socket, press->event_x, press->event_y );
	    	break;
	    	
	    case XCB_BUTTON_RELEASE: 
	    	xcb_button_press_event_t *release;
	    	
	    	release = (xcb_button_press_event_t *) event;
	    	sendMouseUp (socket, release->event_x, release->event_y );
	    	// getMouseLocation (&windowId, &lx, &ly);
	    	// printf ("Inside window %d at (%d, %d)\n", windowId, lx, ly);
	    	break;
	    	
	    case XCB_MOTION_NOTIFY:
	    	xcb_motion_notify_event_t *motion;
	    	motion = (xcb_motion_notify_event_t *)event;
	    	// sendMouseMove (socket, motion->event_x, motion->event_y );
	    	break;
	    	
	    default:
	    	break;
	    }
	}
    }
    catch (std::exception& e)
    {
    	std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
