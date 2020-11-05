client:
	g++ -pthread -I boost/include src/client/* -o client

server:
	g++ -pthread -I boost/include src/server/* -o server

