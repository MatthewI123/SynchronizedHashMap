#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include <vector>
#include "Arguments.hpp"
#include "Logger.hpp"

constexpr const char* USAGE = R"!!(Usage:
	Test [option]... [address] [port]

	If address is unspecified, it defaults to 127.0.0.1.
	If port is unspecified, it defaults to 1000.

Option:
	n	number of clients (default: 10)
	h	shows usage)!!";

int main(int argc, char* argv[])
{
	try {
		auto parsed = ParseArguments<
			2, // option count
			unsigned char, bool, // option types
			std::string, unsigned short // argument types
		>(
			{ 'n', 'h' }, // option names
			{
				4, false, // default option values
				"127.0.0.1", 1000 // default argument values
			},
			argc, argv);

		const auto& options = std::get<0>(parsed);
		const auto& arguments = std::get<1>(parsed);

		auto clients = std::get<0>(options);
		const auto& address = std::get<0>(arguments);
		auto port = std::get<1>(arguments);

		if (std::get<1>(options)) {
			std::cout << USAGE << "\n";
			return 0;
		}

		std::vector<std::thread> threadPool;
		threadPool.reserve(clients);

		std::mt19937 generator;

		for (std::size_t client = 0; client < clients; client++) {
			int gen = std::uniform_int_distribution<int>(0, 2)(generator);

			std::ostringstream stream;
			stream << "./Client";
			stream << " -a " << address;
			stream << " -p " << port;

			if (gen == 0) { // get
				stream << " get";
			} else if (gen == 1) { // post
				stream << " post";
			} else { // delete
				stream << " delete";
			}

			stream << " " << "key" << std::uniform_int_distribution<int>(0, 9)(generator);
			stream << " " << "value" << std::uniform_int_distribution<int>(0, 999)(generator);

			std::cout << "Running " << stream.str().data() << '\n';

			threadPool.emplace_back([](auto stream) {
				Logger logger(stream.str() + ": ");
				system(stream.str().data());
				logger << "done.\n";
				logger.write_out();
			}, std::move(stream));
		}

		for (auto& thread : threadPool)
			thread.join();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
