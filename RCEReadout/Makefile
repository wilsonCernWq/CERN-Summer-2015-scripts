CC = g++
CFLAGS = `root-config --cflags` -O2
LFLAGS = `root-config --ldflags --glibs` -O1
OBJPATH  = obj
SRCPATH  = src
OBJECTS = $(OBJPATH)/RCEReadOut.o $(OBJPATH)/StorageIO.o $(OBJPATH)/Event.o $(OBJPATH)/Track.o $(OBJPATH)/Plane.o $(OBJPATH)/Cluster.o $(OBJPATH)/Hit.o
EXECUTABLE = RCEReadOut

all: RCEReadOut

RCEReadOut: $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(EXECUTABLE)

$(OBJPATH)/RCEReadOut.o: $(SRCPATH)/RCEReadOut.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/RCEReadOut.cpp -o $(OBJPATH)/RCEReadOut.o

$(OBJPATH)/StorageIO.o: $(SRCPATH)/storage/storageio.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/storageio.cpp -o $(OBJPATH)/StorageIO.o

$(OBJPATH)/Event.o: $(SRCPATH)/storage/event.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/event.cpp -o $(OBJPATH)/Event.o

$(OBJPATH)/Track.o: $(SRCPATH)/storage/track.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/track.cpp -o $(OBJPATH)/Track.o

$(OBJPATH)/Plane.o: $(SRCPATH)/storage/plane.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/plane.cpp -o $(OBJPATH)/Plane.o

$(OBJPATH)/Cluster.o: $(SRCPATH)/storage/cluster.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/cluster.cpp -o $(OBJPATH)/Cluster.o
	
$(OBJPATH)/Hit.o: $(SRCPATH)/storage/hit.cpp
	$(CC) $(CFLAGS) -c $(SRCPATH)/storage/hit.cpp -o $(OBJPATH)/Hit.o

clean:
	rm $(OBJPATH)/*.o RCEReadOut
