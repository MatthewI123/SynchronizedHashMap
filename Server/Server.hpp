#pragma once

#include "Constants.hpp"
#include "TCPSocket.hpp"

namespace Network::Server
{
	/** Start listening for connections.
	 */
	inline void Listen(TCPSocket& server, const char* hostname = TCPSocket::ANY_IP, unsigned short port = Constants::PORT, int backlog = 3)
	{
		server.SetAddress(hostname, port);

		if (bind(server.m_socketDescriptor, reinterpret_cast<sockaddr*>(&server.m_address), sizeof(sockaddr_in)) != 0)
			throw std::runtime_error("bind error");

		if (listen(server.m_socketDescriptor, backlog) != 0)
			throw std::runtime_error("listen error");
	}

	/** Accept a connection.
	 */
	inline TCPSocket Accept(TCPSocket& server)
	{
		TCPSocket client(true);
		socklen_t addressSize = sizeof(sockaddr_in);

		if ((client.m_socketDescriptor = accept(server.m_socketDescriptor, reinterpret_cast<sockaddr*>(&client.m_address), &addressSize)) < 0)
			throw std::runtime_error("accept error");

		return client;
	}
}
