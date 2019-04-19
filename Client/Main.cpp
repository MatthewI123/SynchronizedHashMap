#include <iostream>
#include <cerrno>
#include "Constants.hpp"
#include "Client.hpp"
#include "Operation.hpp"
#include "TCPInputOutput.hpp"

extern "C"
{
#include <unistd.h>
}

int main(int argc, char* argv[])
{
	// handle command-line arguments
	bool hasMethod = false, hasKey = false, hasValue = false;
	std::string target("127.0.0.1"), port("1000");
	std::string method, key, value;
	int opt;

	while ((opt = getopt(argc, argv, ":t:p:m:k:v:")) != -1) {
		switch (opt) {
			case 't':
				target = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'm':
				method = optarg;
				hasMethod = true;
				break;
			case 'k':
				key = optarg;
				hasKey = true;
				break;
			case 'v':
				value = optarg;
				hasValue = true;
				break;
			case ':':
				std::cerr << "missing value for " << static_cast<char>(optopt) << "\n";
				exit(1);
				break;
			case '?':
				std::cerr << "unknown option " << static_cast<char>(optopt) << "\n";
				break;
		}
	}

	try {
		Network::TCPSocket socket;
		Network::Operation operation;
		Network::Result result;
		std::string message;

		if (!hasMethod || !hasKey)
			throw std::runtime_error("expected options `m`, `k`");

		if (method == "get")
			operation = Network::Operation::GET;
		else if (method == "post")
			operation = Network::Operation::POST;
		else if (method == "erase")
			operation = Network::Operation::ERASE;
		else
			throw std::runtime_error("bad operation");

		socket << operation;
		socket << key;

		if (operation == Network::Operation::POST && !hasValue)
			throw std::runtime_error("expected option `v`");
		else if (operation == Network::Operation::POST)
			socket << value;

		Network::Client::Connect(socket, target.data(), std::stoi(port));
		Network::Send(socket);
		Network::Receive(socket);

		socket >> result;

		switch (result) {
			case Network::Result::SUCCESS:
				socket >> message;
				std::cout << message << "\n";
				break;
			case Network::Result::REFUSED:
				throw std::runtime_error("server busy");
				break;
			case Network::Result::BAD_KEY:
				throw std::runtime_error("key does not exist");
				break;
			case Network::Result::BAD_OPERATION: // already checked above
			case Network::Result::BAD_PACKET:
			default:
				throw std::runtime_error("unknown error");
				break;
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";

		return 1;
	}

	return 0;
}
