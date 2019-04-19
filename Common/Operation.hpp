#pragma once

namespace Network
{
	enum class Operation : unsigned int
	{
		GET = 0,
		POST = 1,
		ERASE = 2
	};

	enum class Result : unsigned int
	{
		SUCCESS = 0,
		REFUSED = 1, // for too many connections
		BAD_OPERATION = 2, // invalid operation
		BAD_KEY = 3, // invalid key for GET/DELETE
		BAD_PACKET = 4 // malformed paket
	};
}
