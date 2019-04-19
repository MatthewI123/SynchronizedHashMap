#pragma once

namespace Network::Constants
{
	/** The default address the server will be hosted on.
	 * The default address the client will use to connect.
	 */
	constexpr const char* ADDRESS = "127.0.0.1";

	/** The default port the server will be listening to.
	 * The default port the client will use to connect.
	 */
	constexpr unsigned short PORT = 1000;
}
