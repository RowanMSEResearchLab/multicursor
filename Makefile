CC = g++
PKGS = xcb xcb-xtest 
CPPFLAGS = -g -fpermissive `pkg-config --cflags $(PKGS)`
EXTRALIBS= -lboost_system -lpthread
LDFLAGS = `pkg-config --libs $(PKGS)` $(EXTRALIBS)

all: wall dserver sender 

receiver: receiver.o 
	$(CC) -g -o $@ receiver.o $(LDFLAGS)

receiver.o:  receiver.c

wall: wall.o 
	$(CC) -g -o $@ wall.o $(LDFLAGS)

wall.o:  wall.cpp

sender: sender.o 
	$(CC) -g -o $@ sender.o $(LDFLAGS)

sender.o:  sender.cpp


dserver: dserver.o 
	$(CC) -g -o $@ dserver.o $(LDFLAGS)

dserver.o:  dserver.cpp


clean: 
	rm *.o; make 
