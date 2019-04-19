#pragma once

namespace Network
{
	/** Operations the server accepts.
	 */
	enum class Operation : unsigned int
	{
		GET   = 0, // get value
		POST  = 1, // set value
		ERASE = 2, // erase value
	};

	/** Server response codes.
	 */
	enum class Result : unsigned int
	{
		SUCCESS       = 0, // okay [string]
		REFUSED       = 1, // too many connections
		BAD_OPERATION = 2, // invalid operation
		BAD_KEY       = 3, // invalid key for GET/DELETE
		BAD_PACKET    = 4, // malformed paket
	};
}
