#pragma once

#include "TCPSocket.hpp"

namespace Network::Client
{
	/** Connects to a server.
	 */
	inline void Connect(TCPSocket& client, const char* address, unsigned short port)
	{
		client.SetAddress(address, port);

		if (connect(client.m_socketDescriptor, reinterpret_cast<sockaddr*>(&client.m_address), sizeof(sockaddr_in)) < 0)
			throw std::runtime_error("could not connect to server");
	}
}
