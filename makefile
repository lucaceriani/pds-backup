MAKEFLAGS := --jobs=2
.PHONY: client server


CC     = g++ -g
LIB    = -pthread -static -I boost/include
# SHARED = src/shared/*.cpp
BOOST  = -Lboost/lib -lboost_system -lboost_filesystem
SRC    = src


all: client server

clean:
	rm client.out server.out

client:
	$(CC) $(LIB) $(SRC)/client/*.cpp $(SHARED) $(BOOST) -o client.out

server:
	$(CC) $(LIB) $(SRC)/server/*.cpp $(SHARED) $(BOOST) -o server.out

