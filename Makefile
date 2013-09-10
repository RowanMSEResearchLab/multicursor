CC = g++
PKGS = xcb xcb-xtest 
CPPFLAGS = -g -fpermissive `pkg-config --cflags $(PKGS)`
EXTRALIBS= -lboost_system -lboost_thread -lpthread 
LDFLAGS = `pkg-config --libs $(PKGS)` $(EXTRALIBS)

all: wall sender 

receiver: receiver.o 
	$(CC) -g -o $@ receiver.o $(LDFLAGS)

receiver.o:  receiver.c

wall: wall.o  xcbutil.o serverthread.o vcursor.o enforcer.o
	$(CC) -g -o $@ wall.o xcbutil.o serverthread.o vcursor.o enforcer.o $(LDFLAGS)

wall.o:  wall.cpp xcbutil.h mouse.h serverthread.h enforcer.h

sender: sender.o xcbutil.o
	$(CC) -g -o $@ sender.o xcbutil.o $(LDFLAGS)

sender.o:  sender.cpp xcbutil.h mouse.h

serverthread.o:  serverthread.cpp serverthread.h xcbutil.h mouse.h enforcer.h

vcursor.o: vcursor.cpp vcursor.h xcbutil.h mouse.h

enforcer.o: enforcer.cpp enforcer.h xcbutil.h

xcbutil.o : xcbutil.h xcbutil.cpp

clean: 
	rm *.o; make 
