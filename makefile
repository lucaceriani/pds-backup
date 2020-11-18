MAKEFLAGS := --jobs=2
.PHONY: client server


CC     = g++
LIB    = -pthread -I boost/include 
SHARED = src/shared/*.cpp
SRC    = src

default: client server

client:
	$(CC) $(LIB) $(SRC)/client/*.cpp $(SHARED) -o client.out

server:
	$(CC) $(LIB) $(SRC)/server/*.cpp $(SHARED) -o server.out

