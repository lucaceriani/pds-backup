MAKEFLAGS := --jobs=2
.PHONY: client server


CC     = g++
LIB    = -pthread -I boost/include
# SHARED = src/shared/*.cpp
SRC    = src


all: client server

clean:
	rm client.out server.out

client:
	$(CC) $(LIB) $(SRC)/client/*.cpp $(SHARED) -static -Lboost/lib -lboost_system -lboost_filesystem -o client.out

server:
	$(CC) $(LIB) $(SRC)/server/*.cpp $(SHARED) -o server.out

