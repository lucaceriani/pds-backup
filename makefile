MAKEFLAGS := --jobs=2

.PHONY: client server

default: client server

client:
	g++ -pthread -I boost/include src/client/* -o client.out

server:
	g++ -pthread -I boost/include src/server/* -o server.out

