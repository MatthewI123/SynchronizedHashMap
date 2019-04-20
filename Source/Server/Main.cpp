#include <iostream>
#include "Arguments.hpp"
#include "Operation.hpp"
#include "Server.hpp"
#include "TCPInputOutput.hpp"

constexpr const char* USAGE = R"!!(Usage:
	Server [option]... [address] [port]

	If address is unspecified, it defaults to 0.0.0.0 (0.0.0.0 for any interface).
	If port is unspecified, it defaults to 1000 (0 for any port).

Option:
	t	number of threads (default: 4)
	h	shows usage)!!";

int main(int argc, char* argv[])
{
	auto parsed = ParseArguments<
		2, // option count
		unsigned char, bool, // options
		std::string, unsigned short // arguments
		>(
		{ 't', 'h' }, // option names
		{
			4, false, // default option values
			"0.0.0.0", 4 // default argument values
		},
		argc, argv);

	const auto& options = std::get<0>(parsed);
	const auto& arguments = std::get<1>(parsed);

	auto threads = std::get<0>(options);
	auto address = std::get<0>(arguments);
	auto port = std::get<1>(arguments);

	std::cout << "Number of threads: " << static_cast<int>(threads) << "\n";
	std::cout << "Address: " << address << "\n";
	std::cout << "Port: " << port << "\n";

	if (std::get<1>(options)) {
		std::cout << USAGE << "\n";
		return 0;
	}

/*	try {
		Network::TCPSocket server;

		Network::Server::Listen(server, Network::TCPSocket::ANY_IP, 1000, 3);
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
					client >> value;
					std::cout << "POST " << key << " = " << value << "\n";
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
*/
	return 0;
}
