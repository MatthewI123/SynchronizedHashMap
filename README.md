# Client/Server Hashtable Project

## CSC 415 - Matthew


# Running

1. Build the project by invoking `make`.
2. Start a server by invoking `./Server`
3. Start a client by invoking `./Client post MyKey MyValue`


# Server

```
Usage:
	Server [option]... [address] [port]

	If address is unspecified, it defaults to 0.0.0.0 (0.0.0.0 for any interface).
	If port is unspecified, it defaults to 1000 (0 for any port).

Option:
	t	number of threads (default: 4)
	h	shows usage
```


# Client

```
Usage:
	Client [option]... <get | delete> <key>
	Client [option]... post <key> <value>

Option:
	a	server address (default: 127.0.0.1)
	p	server port (default: 1000)
	h	shows usage
```


# Test

```
Usage:
	Test [option]... [address] [port]

	If address is unspecified, it defaults to 127.0.0.1.
	If port is unspecified, it defaults to 1000.

Option:
	n	number of clients (default: 10)
	h	shows usage
```
