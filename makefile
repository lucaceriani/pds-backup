MAKEFLAGS := --jobs=$(shell nproc)
.PHONY: clean

DBG    = -g
CFLAGS = -std=c++17 -Wall -pthread $(DBG)

SRC_SERVER = $(wildcard src/server/*.cpp)
SRC_SHARED = $(wildcard src/shared/*.cpp)
SRC_CLIENT = $(wildcard src/client/*.cpp)

OBJ_SERVER = $(SRC_SERVER:%.cpp=out/%.o) $(SRC_SHARED:%.cpp=out/%.o)
OBJ_CLIENT = $(SRC_CLIENT:%.cpp=out/%.o) $(SRC_SHARED:%.cpp=out/%.o)

BOOST      = -Iboost/include -Lboost/lib -lboost_system -lboost_filesystem -lboost_random
OSSL       = -lssl -lcrypto
LDFLAGS    = $(BOOST) $(OSSL)

all: client server

# regola per creare i file oggetto
out/%.o: %.cpp 
	@mkdir -p $(dir $@)
	@echo "building --> $^"
	@$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

server: $(OBJ_SERVER)
	@echo "linking ---> server"
	@$(CXX) $(CFLAGS) -o out/$@ $^ $(LDFLAGS)

client: $(OBJ_CLIENT)
	@echo "linking ---> client"
	@$(CXX) $(CFLAGS) -o out/$@ $^ $(LDFLAGS)

clean:
	@rm -rf out

sc: #start client
	@export LD_LIBRARY_PATH=LD_LIBRARY_PATH:"$(shell pwd)/boost/lib" && ./out/client localhost 1234 __to_watch

ss: #start server
	@export LD_LIBRARY_PATH=LD_LIBRARY_PATH:"$(shell pwd)/boost/lib" && ./out/server
