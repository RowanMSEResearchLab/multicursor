CC = g++
PKGS = xcb xcb-xtest 
CPPFLAGS = -g -fpermissive `pkg-config --cflags $(PKGS)`
EXTRALIBS= -lboost_system -lpthread
LDFLAGS = `pkg-config --libs $(PKGS)` $(EXTRALIBS)

all: wall sender 

receiver: receiver.o 
	$(CC) -g -o $@ receiver.o $(LDFLAGS)

receiver.o:  receiver.c

wall: wall.o  xcbutil.o
	$(CC) -g -o $@ wall.o xcbutil.o $(LDFLAGS)

wall.o:  wall.cpp xcbutil.h

sender: sender.o xcbutil.o
	$(CC) -g -o $@ sender.o xcbutil.o $(LDFLAGS)

sender.o:  sender.cpp xcbutil.h


dserver: dserver.o 
	$(CC) -g -o $@ dserver.o $(LDFLAGS)

dserver.o:  dserver.cpp


clean: 
	rm *.o; make 
