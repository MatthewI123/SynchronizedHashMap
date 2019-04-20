.PHONY: all client server clean

CXX := clang++ -Wall -Werror -pedantic -O2 -std=c++17 -IInclude/ -lpthread

all: server client

clean:
	rm -f Server Client

server:
	$(CXX) -DHASHTABLE_SERVER Source/Server/*.cpp -o Server

client:
	$(CXX) -DHASHTABLE_CLIENT Source/Client/*.cpp -o Client
