#include <iostream>
#include <cerrno>
#include <thread>
#include "Operation.hpp"
#include "Server.hpp"
#include "TCPInputOutput.hpp"

int main()
{
	try {
		Network::TCPSocket server;

		Network::Server::Listen(server);
		std::cout << "Listening on port " << server.GetPort() << "...\n";

		while (true) {
			Network::TCPSocket client = Network::Server::Accept(server);

			std::cout << "Client (" << client.GetIPAddress() << ":" << client.GetPort() << ") accepted!\n";

			Network::Receive(client);
			Network::Operation operation;
			std::string key;
			std::string value;

			client >> operation;
			client >> key;

			switch (operation) {
				case Network::Operation::GET:
					std::cout << "GET " << key << "\n";
					client << Network::Result::BAD_KEY;
					break;
				case Network::Operation::POST:
					std::cout << "POST " << key << " = " << value << "\n";
					client >> value;
					client << Network::Result::BAD_KEY;
					break;
				case Network::Operation::ERASE:
					std::cout << "ERASE " << key << "\n";
					client << Network::Result::BAD_KEY;
					break;
				default:
					std::cerr << "Bad Operation\n";
					client << Network::Result::BAD_OPERATION;
					break;
			}

			Network::Send(client);

		}

		std::cout << "Success.\n";
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";

		return 1;
	}

	return 0;
}
