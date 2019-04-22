#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include <vector>
#include "Arguments.hpp"
#include "ThreadPool.hpp"

constexpr const char* USAGE = R"!!(Usage:
	Test [option]... [address] [port]

	If address is unspecified, it defaults to 127.0.0.1.
	If port is unspecified, it defaults to 1000.

Option:
	n	number of clients (default: 10)
	t       number of threads (default: 4)
	h	shows usage)!!";

int main(int argc, char* argv[])
{
	try {
		auto parsed = ParseArguments<
			3, // option count
			unsigned char, unsigned char, bool, // option types
			std::string, unsigned short // argument types
		>(
			{ 'n', 't', 'h' }, // option names
			{
				10, 4, false, // default option values
				"127.0.0.1", 1000 // default argument values
			},
			argc, argv);

		const auto& options = std::get<0>(parsed);
		const auto& arguments = std::get<1>(parsed);

		auto clients = std::get<0>(options);
		auto threads = std::get<1>(options);
		const auto& address = std::get<0>(arguments);
		auto port = std::get<1>(arguments);

		if (std::get<2>(options)) {
			std::cout << USAGE << "\n";
			return 0;
		}

		if (threads == 0)
			throw std::runtime_error("at least 1 thread is required");

		ThreadPool threadPool(threads);

		std::mt19937 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());

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

			if (threadPool.CanQueue()) {
				threadPool.Queue([](auto stream) {
					system(stream.str().data());
				}, std::move(stream));
			} else {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		threadPool.JoinAll();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
