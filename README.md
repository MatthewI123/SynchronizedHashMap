# Client/Server Hashtable Project
## CSC 415 - Matthew


# Running
1. Build the project by invoking `make`.
2. Start a server by invoking `./Server.out`
3. Start a client by invoking `./Client.out -m post -k MyKey -v MyValue`


# Server

```
Usage:
	Server.out [option]...

Option:
	t	target host (default: 127.0.0.1)
	p	port (default: 1000)
	n	number of threads (default: 4) TODO
```


# Client

```
Usage:
	Client.out [option]...

Option:
	t	server host (default: 127.0.0.1)
	p	server port (default: 1000)
	m	method (required, either `get`, `post`, `erase`)
	k	key (required)
	v	value (required if method is `post`)
```
