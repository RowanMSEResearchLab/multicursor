CC = gcc
PKGS = xcb xcb-xtest
CFLAGS = -g -Wall `pkg-config --cflags $(PKGS)`
LDFLAGS = `pkg-config --libs $(PKGS)`

all: receiver

receiver: receiver.o 
	$(CC) -g -o $@ receiver.o $(LDFLAGS)

receiver.o:  receiver.c

clean: 
	rm *.o; make 
