#pragma once

#include "TCPSocket.hpp"

namespace Network
{
	/** Sends the write stream over the socket and clears it.
	 */
	inline void Send(TCPSocket& socket)
	{
		const std::string& content = socket.m_write_stream;
		std::size_t headerSize = sizeof(std::size_t);
		std::size_t contentLength = Traits::template hton<std::size_t>::func(content.size());
		std::size_t written = 0;

		while (written != headerSize + content.size()) {
			const char* pointer;
			std::size_t size;

			if (written < headerSize) { // write header
				pointer = reinterpret_cast<const char*>(&contentLength) + written;
				size = headerSize - written;
			} else { // write content
				std::size_t index = written - headerSize;
				pointer = &content[index];
				size = content.size() - index;
			}

			auto count = write(socket.m_socketDescriptor, pointer, size);
			written += count;

			if (count < 0)
				throw std::runtime_error("error writing");
		}

		socket.m_write_stream.clear();
	}

	/** Reads from the socket into the read stream.
	 */
	void Receive(TCPSocket& socket)
	{
		std::string& content = socket.m_read_stream;
		content.clear();
		socket.m_read_pos = 0;

		std::size_t headerSize = sizeof(std::size_t);
		std::size_t contentLength = 0;
		std::size_t seen = 0;
		std::size_t contentLengthRead;

		while (seen < headerSize + contentLength) {
			char* pointer;
			std::size_t size;

			if (seen < headerSize) { // seen header
				pointer = reinterpret_cast<char*>(&contentLengthRead) + seen;
				size = headerSize - seen;
			} else { // read content
				std::size_t index = seen - headerSize;
				pointer = &content[index];
				size = contentLength - index;
			}

			auto count = read(socket.m_socketDescriptor, pointer, size);
			seen += count;

			if (count < 0)
				throw std::runtime_error("error reading");

			if (seen == headerSize && count != 0) {
				contentLength = Traits::template ntoh<std::size_t>::func(contentLengthRead);
				content.resize(contentLength);
			}
		}
	}
}
