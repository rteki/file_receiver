EXENAME= frc

CC=g++

CFLAGS=-c

CLIBS+=-std=c++11

ifeq ($(OS),Windows_NT)
	CLIBS+= -lws2_32
	EXENAME:=$(EXENAME).exe
endif

all:$(EXENAME)

$(EXENAME): main.o filerecv.o directoryexplorer.o direxpclient.o direxpserver.o fileinfo.cpp socket.o
	$(CC) main.o filerecv.o directoryexplorer.o direxpclient.o direxpserver.o fileinfo.cpp socket.o $(CLIBS) -o $(EXENAME)
	
filerecv.o: filerecv.cpp
	$(CC) $(CFLAGS) filerecv.cpp $(CLIBS)
	
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp  $(CLIBS)

directoryexplorer.o: directoryexplorer.cpp
	$(CC) $(CFLAGS) directoryexplorer.cpp $(CLIBS)
	
direxpclient.o: direxpclient.cpp
	$(CC) $(CFLAGS) direxpclient.cpp $(CLIBS)
	
direxpserver.o: direxpserver.cpp
	$(CC) $(CFLAGS) direxpserver.cpp $(CLIBS)
	
fileinfo.o: fileinfo.cpp
	$(CC) $(CFLAGS) fileinfo.cpp $(CLIBS)
	
socket.o: socket.cpp
	$(CC) $(CFLAGS) socket.cpp $(CLIBS)
	
DELETE= rm

ifeq ($(OS),Windows_NT)
	DELETE= del
endif

	
cleanall:
	$(DELETE) *.o $(EXENAME)
	
clean:
	$(DELETE) *.o