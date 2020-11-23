MAKEFLAGS := --jobs=2
.PHONY: client server


CC     = g++ 
LIB    = -pthread -static
SHARED = src/shared/*.cpp
BOOST = -Lboost/lib -lboost_system -lboost_filesystem
SRC    = src


all: client server

clean:
	rm client.out server.out

client:
	$(CC) $(LIB) $(SHARED) $(SRC)/client/*.cpp $(BOOST) -o client.out

server:
	$(CC) $(LIB) $(SHARED) $(SRC)/server/*.cpp $(BOOST) -o server.out

