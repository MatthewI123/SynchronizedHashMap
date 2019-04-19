#pragma once

#include "Constants.hpp"
#include "TCPSocket.hpp"

namespace Network::Client
{
	/** Connect to server.
	 */
	inline void Connect(TCPSocket& client, const char* hostname = Constants::ADDRESS, unsigned short port = Constants::PORT)
	{
		client.SetAddress(hostname, port);

		if (connect(client.m_socketDescriptor, reinterpret_cast<sockaddr*>(&client.m_address), sizeof(sockaddr_in)) < 0)
			throw std::runtime_error("could not connect to server");
	}
}
