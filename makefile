MAKEFLAGS := --jobs=2
.PHONY: client server


CC     = g++ 
LIB    = -pthread -static #-Iboost/include -Lboost/lib -lboost_system -lboost_filesystem
SHARED = src/shared/*.cpp
BOOST = -Lboost/lib -lboost_system -lboost_filesystem
SRC    = src


all: client server

clean:
	rm client.out server.out

client:
	$(CC) $(LIB) $(SRC)/client/*.cpp $(BOOST) -o client.out

server:
	$(CC) $(LIB) $(SRC)/server/*.cpp $(BOOST) -o server.out

