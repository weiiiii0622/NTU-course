.PHONY: all clean

all: server client
server: server.cpp
	g++ -std=c++17 -Wall -o server server.cpp utils/base64.c
client: client.cpp
	g++ -std=c++17 -Wall -o client client.cpp utils/base64.c
clean:
	@rm -rf server client