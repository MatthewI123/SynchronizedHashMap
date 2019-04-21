#include <iostream>
#include <list>
#include <thread>
#include "Arguments.hpp"
#include "Hashtable.hpp"
#include "Logger.hpp"
#include "Operation.hpp"
#include "Server.hpp"
#include "TCPInputOutput.hpp"
#include "ThreadPool.hpp"

constexpr const char* USAGE = R"!!(Usage:
	Server [option]... [address] [port]

	If address is unspecified, it defaults to 0.0.0.0 (0.0.0.0 for any interface).
	If port is unspecified, it defaults to 1000 (0 for any port).

Option:
	t	number of threads (default: 4)
	h	shows usage)!!";

int main(int argc, char* argv[])
{
	try {
		auto parsed = ParseArguments<
			2, // option count
			unsigned char, bool, // option types
			std::string, unsigned short // argument types
			>(
				{ 't', 'h' }, // option names
				{
					4, false, // default option values
					"0.0.0.0", 1000 // default argument values
				},
				argc, argv);

		const auto& options = std::get<0>(parsed);
		const auto& arguments = std::get<1>(parsed);

		auto threads = std::get<0>(options);
		const auto& address = std::get<0>(arguments);
		auto port = std::get<1>(arguments);

		if (std::get<1>(options)) {
			std::cout << USAGE << "\n";
			return 0;
		}

		if (threads == 0)
			throw std::runtime_error("at least 1 thread is required");

		Logger logger("[Server] ");
		ThreadPool threadPool(threads);
		Hashtable<std::string, std::string> table;

		Network::TCPSocket server;

		Network::Server::Listen(server,
			address == "0.0.0.0" ? Network::TCPSocket::ANY_IP : address.data(), port, 3);
		logger << "Listening on port " << server.GetPort() << "...\n";
		logger.write_out();

		while (true) {
			if (threadPool.CanQueue()) {
				Network::TCPSocket client { Network::Server::Accept(server) };
				logger << "Client (" << client.GetIPAddress() << ":" << client.GetPort() << ") accepted!\n";
				logger.write_out();

				threadPool.Queue([&table](auto client) mutable {
					Logger logger("[Client] ");

					try {
						Network::Receive(client);
						Network::Operation operation;
						std::string key;
						std::string value;

						client >> operation;
						client >> key;

						switch (operation) {
							case Network::Operation::GET:
								if (auto val = table.Get(key);
								    val.has_value()) {
									logger << "GET " << key << " success.\n";
									client << Network::Result::SUCCESS;
									client << val.value();
								} else {
									logger << "GET " << key << " fail.\n";
									client << Network::Result::BAD_KEY;
								}
								break;
							case Network::Operation::POST:
								client >> value;
								logger << "POST " << key << " = " << value << ".\n";
								table.Set(key, value);
								client << Network::Result::SUCCESS;
								break;
							case Network::Operation::DELETE:
								if (table.Delete(key)) {
									logger << "DELETE " << key << " success.\n";
									client << Network::Result::SUCCESS;
								} else {
									logger << "DELETE " << key << " fail.\n";
									client << Network::Result::BAD_KEY;
								}
								break;
							default:
								logger << "BAD OP " << static_cast<std::underlying_type_t<Network::Operation>>(operation) << ".\n";
								std::cerr << "Bad Operation\n";
								client << Network::Result::BAD_OPERATION;
								break;
						}

						logger.write_out();

						// pretend it takes a while
						std::this_thread::sleep_for(std::chrono::seconds(2));

						Network::Send(client);
					} catch (const std::exception& e) {
						logger << e.what() << "\n";
						logger.write_err();
					}
				}, std::move(client));
			} else {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		threadPool.JoinAll();
	} catch (const std::invalid_argument& e) {
		std::cerr << "invalid argument type\n";
		return 1;
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
