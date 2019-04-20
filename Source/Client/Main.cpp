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
	p	server port (default: 1000))!!";

auto HandleCLAs(int argc, char* argv[])
{
	std::tuple<std::string, unsigned short, Network::Operation, std::string, std::string> values;
	int opt, n;

	std::get<0>(values) = "127.0.0.1";
	std::get<1>(values) = 1000;

	while ((opt = getopt(argc, argv, ":a:p:")) != -1) {
		if (opt == 'a') {
			std::get<0>(values) = optarg;
		} else if (opt == 'p') {
			using range = std::numeric_limits<unsigned short>;
			int port = std::stoi(optarg);

			if (port < range::min() || port > range::max())
				throw std::runtime_error("port out of range");
			std::get<1>(values) = static_cast<unsigned short>(port);
		} else if (opt == ':') {
			throw std::runtime_error(std::string("option '") + static_cast<char>(optopt) + "' missing");
		} else if (opt == '?') {
			std::cerr << "ignoring option '" << static_cast<char>(optopt) << "'\n";
		}
	}

	n = argc - optind;

	if (n >= 2) {
		std::string_view operation = argv[optind];

		if (operation == "get")
			std::get<2>(values) = Network::Operation::GET;
		else if (operation == "post")
			std::get<2>(values) = Network::Operation::POST;
		else if (operation == "erase")
			std::get<2>(values) = Network::Operation::ERASE;
		else
			throw std::runtime_error(std::string("unknown operation '") + operation.data() + '\'');

		std::get<3>(values) = argv[optind + 1];

		if (std::get<2>(values) == Network::Operation::POST) {
			if (n < 3)
				throw std::runtime_error("expected value field for the post operation");
			else
				std::get<4>(values) = argv[optind + 2];
		}
	} else {
		throw std::runtime_error("expected operation and key fields");
	}

	return values;
}

int main(int argc, char* argv[])
{
	if (argc == 1) { // print usage
		std::cout << USAGE << '\n';
		return 0;
	}

	try {
		auto arguments = HandleCLAs(argc, argv);

		Network::TCPSocket socket;
		Network::Result result;

		Network::Client::Connect(socket, std::get<0>(arguments).data(), std::get<1>(arguments));

		socket << std::get<2>(arguments); // operation
		socket << std::get<3>(arguments); // key

		if (std::get<2>(arguments) == Network::Operation::POST)
			socket << std::get<4>(arguments); // value

		Network::Send(socket);
		Network::Receive(socket);

		socket >> result;

		if (result == Network::Result::SUCCESS) {
			std::string message;
			socket >> message;
			std::cout << message << '\n';
		} else if (result == Network::Result::REFUSED) {
			std::cout << "server busy\n";
		} else if (result == Network::Result::BAD_KEY) {
			std::cout << "<unknown key>\n";
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
