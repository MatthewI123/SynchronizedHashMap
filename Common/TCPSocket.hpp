#pragma once

#include <string_view>
#include <type_traits>
#include "Traits.hpp"

extern "C"
{
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
}

namespace Network
{
	/** The TCPSocket class encapsulates the socket object and provides an interface around it.
	 * It supports both reading and writing POD types as well as buffers.
	 */
	class TCPSocket
	{
	public:
		static constexpr const char* ANY_IP = INADDR_ANY;

		int m_socketDescriptor;
		sockaddr_in m_address;
		std::string m_read_stream;
		std::string m_write_stream;
		std::size_t m_read_pos = 0;

		/** Constructs a TCPSocket with a new socket.
		 */
		explicit TCPSocket(bool dontCreate = false)
		{
			if (!dontCreate) {
				int optval = 1;

				if ((m_socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
					throw std::runtime_error("could not create socket");

				if (setsockopt(m_socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) != 0)
					throw std::runtime_error("could not set socket options");
			}
		}

		/** TCPSocket destructor.
		 */
		~TCPSocket()
		{
			shutdown(m_socketDescriptor, SHUT_RDWR);
			close(m_socketDescriptor);
		}

		/** Assigns an address to the socket.
		 */
		void SetAddress(const char* hostname, unsigned short port)
		{
			if (hostname == ANY_IP) {
				m_address.sin_addr.s_addr = INADDR_ANY;
			} else {
				if (inet_pton(AF_INET, hostname, &m_address.sin_addr) != 1)
					throw std::runtime_error("could not resolve IP from given hostname");
			}

			m_address.sin_family = AF_INET;
			m_address.sin_port = htobe16(port);
		}

		/** Gets a readable address.
		 */
		std::string GetIPAddress() const
		{
			char address[INET_ADDRSTRLEN];

			if(inet_ntop(AF_INET, &m_address.sin_addr, address, INET_ADDRSTRLEN) == NULL)
				throw std::runtime_error("could not fetch IP");

			return std::string(address);
		}

		unsigned short GetPort() const noexcept
		{
			return be16toh(m_address.sin_port);
		}

		void Write(std::string& stream, const void* value, std::size_t size)
		{
			m_write_stream.resize(m_write_stream.size() + size);
			std::copy(reinterpret_cast<const char*>(value), reinterpret_cast<const char*>(value) + size, m_write_stream.begin() + m_write_stream.size() - size);
		}

		void Read(void* buffer, std::size_t size)
		{
			const char* begin = m_read_stream.data() + m_read_pos;

			if (begin + size <= m_read_stream.data() + m_read_stream.size()) {
				std::copy(begin, begin + size, reinterpret_cast<char*>(buffer));
				m_read_pos += size;
			} else {
				throw std::runtime_error("unexpected end of stream");
			}
		}


		/** Writes an arithmetic or buffer-like type to the output stream.
		 */
		template<typename T>
		TCPSocket& operator<<(const T& value)
		{
			if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
				T serialized = static_cast<T>(Traits::template hton<T>::func(value));

				Write(m_write_stream, reinterpret_cast<const char*>(&serialized), sizeof(T));
			} else if constexpr (std::is_constructible_v<std::string_view, T>) {
				std::string_view buff(value);
				std::size_t size = Traits::template hton<std::size_t>::func(buff.size());

				Write(m_write_stream, reinterpret_cast<const char*>(&size), sizeof(std::size_t));
				Write(m_write_stream, buff.data(), buff.size());
			} else {
				static_assert(Traits::dependent_false<T>::value, "TCPSocket::Write<T>(const T&) - unsupported type T");
			}

			return *this;
		}

		/** Reads an arithmetic or buffer-like type from the input stream
		 */
		template<typename T>
		TCPSocket& operator>>(T& value)
		{
			if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
				Read(reinterpret_cast<char*>(&value), sizeof(T));
				value = static_cast<T>(Traits::template ntoh<T>::func(value));
			} else if constexpr (std::is_assignable_v<T, std::string>) {
				std::string buffer;
				std::size_t size;

				Read(reinterpret_cast<char*>(&size), sizeof(std::size_t));
				size = Traits::template ntoh<std::size_t>::func(size);

				buffer.resize(size);
				Read(buffer.data(), size);

				value = std::move(buffer);
			} else {
				static_assert(Traits::dependent_false<T>::value, "TCPSocket::Read<T>() - unsupported type T");
			}

			return *this;
		}
	};
}
