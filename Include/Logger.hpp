#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <sstream>

class Logger
{
private:
	std::string m_prefix;
	std::stringstream m_stream;

	static std::mutex m_mutex;

public:
	Logger(std::string prefix)
	: m_prefix(std::move(prefix))
	{
	}

	template<typename T>
	Logger& operator<<(T&& value)
	{
		m_stream << value;
		return *this;
	}

	void write_out() noexcept
	{
		std::string str = m_prefix + m_stream.str();

		m_mutex.lock();
		std::cout << str;
		std::cout.flush();
		m_mutex.unlock();

		m_stream.str("");
	}

	void write_err() noexcept
	{
		std::string str = m_prefix + m_stream.str();

		m_mutex.lock();
		std::cerr << str;
		std::cerr.flush();
		m_mutex.unlock();

		m_stream.str("");
	}
};

std::mutex Logger::m_mutex;
