.PHONY: all server client test clean

CXX := clang++ -Wall -pedantic -O2 -std=c++17 -IInclude/ -lpthread

all: server client test

clean:
	rm -f Server Client Test

server:
	$(CXX) -DHASHTABLE_SERVER Source/Server/*.cpp -o Server

client:
	$(CXX) -DHASHTABLE_CLIENT Source/Client/*.cpp -o Client

test:
	$(CXX) Source/Test.cpp -o Test
