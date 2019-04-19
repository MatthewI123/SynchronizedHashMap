.PHONY: all client server clean

CXX := g++ -Wall -Werror -pedantic -O2 -std=c++17 -ICommon/ -lpthread

all: server client

clean:
	rm -f Server.out Client.out

server:
	$(CXX) Server/*.cpp -o Server.out

client:
	$(CXX) Client/*.cpp -o Client.out
