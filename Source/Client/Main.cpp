#include <iostream>
#include "Arguments.hpp"
#include "Client.hpp"
#include "Operation.hpp"
#include "TCPInputOutput.hpp"

constexpr const char* USAGE = R"!!(Usage:
	Client [option]... <get | erase> <key>
	Client [option]... post <key> <value>

Option:
	a	server address (default: 127.0.0.1)
	p	server port (default: 1000)
	h	shows usage)!!";

int main(int argc, char* argv[])
{
	try {
		auto parsed = ParseArguments<
			3, // option count
			std::string, unsigned short, bool, // option types
			std::optional<std::string>, std::optional<std::string>, std::optional<std::string> // argument types
		>(
			{ 'a', 'p', 'h' }, // option names
			{
				"127.0.0.1", 1000, false, // default option values
				{ }, { }, { } // default argument values
			},
			argc, argv);

		const auto& options = std::get<0>(parsed);
		const auto& arguments = std::get<1>(parsed);

		const auto& address = std::get<0>(options);
		auto port = std::get<1>(options);
		const auto& operation = std::get<0>(arguments);
		const auto& key = std::get<1>(arguments);
		const auto& value = std::get<2>(arguments);
		Network::Operation op;

		if (std::get<2>(options)) {
			std::cout << USAGE << "\n";
			return 0;
		}

		if (!operation.has_value())
			throw std::runtime_error("expected operation argument");
		if (!key.has_value())
			throw std::runtime_error("expected key argument");

		if (operation == "get")
			op = Network::Operation::GET;
		else if (operation == "post")
			op = Network::Operation::POST;
		else if (operation == "delete")
			op = Network::Operation::DELETE;
		else
			throw std::runtime_error("bad operation");

		if (op == Network::Operation::POST && !value.has_value())
			throw std::runtime_error("expected value for post");

		Network::TCPSocket socket;
		Network::Result result;


		socket << op; // operation
		socket << key.value();

		if (op == Network::Operation::POST)
			socket << value.value();

		Network::Client::Connect(socket, address.data(), port);
		Network::Send(socket);
		Network::Receive(socket);

		socket >> result;

		if (result == Network::Result::SUCCESS) {
			std::cout << "Success";

			if (op == Network::Operation::GET) {
				std::string message;
				socket >> message;
				std::cout << ": " << message;
			}

			std::cout << '\n';
		} else if (result == Network::Result::REFUSED) {
			std::cout << "server busy\n";
		} else if (result == Network::Result::BAD_KEY) {
			std::cout << "** could not get/delete key, it does not exist! **\n";
		} else {
			/* already checked for valid operation in HandleCLAs */
			/* already checked for missing value in HandleCLAs */
			std::cout << "unknown error\n";
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
