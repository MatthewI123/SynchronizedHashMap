.PHONY: all client server clean

CXX := g++ -Wall -Werror -pedantic -O2 -std=c++17 -ICommon/ -lpthread

all: server client

clean:
	rm -f Server.out Client.out

server:
	$(CXX) -DHASHTABLE_SERVER Server/*.cpp -o Server.out

client:
	$(CXX) -DHASHTABLE_CLIENT Client/*.cpp -o Client.out
