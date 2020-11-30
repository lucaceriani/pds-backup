MAKEFLAGS := --jobs=4
#.PHONY: client server


CC     = g++ -std=c++17
LIB    = -pthread 
SHARED = src/shared/*.cpp
DBG    = -g
BOOST  = -Iboost/include -Lboost/lib -lboost_system -lboost_filesystem
OSSL   = -lssl -lcrypto
SRC    = src

all: client server

clean:
	rm client.out server.out

client:
	$(CC) $(DBG) $(LIB) $(SHARED) $(SRC)/client/*.cpp $(BOOST) $(OSSL) -o client.out

server:
	$(CC) $(DBG) $(LIB) $(SHARED) $(SRC)/server/*.cpp $(BOOST) $(OSSL) -o server.out

sc: #start server
	@export LD_LIBRARY_PATH=LD_LIBRARY_PATH:"$(shell pwd)/boost/lib" && ./client.out

ss: #start client
	@export LD_LIBRARY_PATH=LD_LIBRARY_PATH:"$(shell pwd)/boost/lib" && ./server.out	
