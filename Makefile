.PHONY: all server client test clean

CXX := g++ -Wall -pedantic -O2 -std=c++17 -IInclude/

all: server client test

clean:
	rm -f Server Client Test

server:
	$(CXX) -DHASHTABLE_SERVER Source/Server/*.cpp -lpthread -o Server

client:
	$(CXX) -DHASHTABLE_CLIENT Source/Client/*.cpp -o Client

test:
	$(CXX) Source/Test.cpp -lpthread -o Test
