# todos

## [fa60ddc] solve initialize to 0 issue with struct sockaddr_in 
```bash
main.cpp:25:23: error: missing field 'sin_port' initializer [-Werror,-Wmissing-field-initializers]
        sockaddr_in serv = {0};
```
```cpp
// change code to value-initialize
sockaddr_in serv = sockaddr_in();

// related 
// value-initialization in c++98
// default ctor
```



## [b5485a3] make socket file descriptor async
```cpp
// accept
// conn1
int conn1 = accept(sock, NULL, NULL);
if (conn1 < 0) {
	std::cout << "Error accept\n";
	return 5;
}

// have conn2
int conn2 = accept(sock, NULL, NULL);
if (conn2 < 0) {
	std::cout << "Error accept\n";
	return 6;
}
```
```cpp
// get current flag then add 0_NONBLOCK
int flags = fcntl(sock, F_GETFL, 0);
if (flags < 0) {
	std::cout << "Error F_GETFL\n";
	return 1;
}
retval = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
if (retval < 0) {
	std::cout << "Error F_SETFL\n";
	return 1;
}

// ...

// accept
// conn1
int conn1 = accept(sock, NULL, NULL);
while (conn1 < 0) {
	if (conn1 < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// no pending connection
			// ask again
			conn1 = accept(sock, NULL, NULL);
		}
		else {
			std::cout << "Error conn1\n";
			return 1;
		}
	}
}


// conn2
int conn2 = accept(sock, NULL, NULL);
while (conn2 < 0) {
	if (conn2 < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// no pending connection
			// ask again
			conn2 = accept(sock, NULL, NULL);
		}
		else {
			std::cout << "Error conn2\n";
			return 1;
		}
	}
}
```



## [0032ef7] use errno in cpp
```cpp
#include <errno.h>

// subject require cpp equivelent of c library if possible
```

```cpp
#include <cerrno>
```

## [ab43a4c] make connected sockets nonblocking
```cpp
while (1) {
	// still blocks conn1 doesnt inherit from sock directly
	size = recv(conn1, buff, 511, 0);
	if (size > 0) {
		buff[size] = '\0';
		size = send(conn2, buff, size, 0);
		if (size < 0) {
			std::cout << "Error send\n";
			return 1;
		}
	}
	// still blocks conn2 doesnt become non blocking automatically
	size = recv(conn2, buff, 511, 0);
	if (size > 0) {
		buff[size] = '\0';
		size = send(conn1, buff, size, 0);
		if (size < 0) {
			std::cout << "Error send\n";
			return 1;
		}
	}
}
```

```cpp
// make conns non blocking
flags = fcntl(conn1, F_GETFL, 0);
if (flags < 0) {
	std::cout << "Error conn1 F_GETFL\n";
	return 1;
}
retval = fcntl(conn1, F_SETFL, flags | O_NONBLOCK);
if (retval < 0) {
	std::cout << "Error conn1 F_SETFL\n";
	return 1;
}

flags = fcntl(conn2, F_GETFL, 0);
if (flags < 0) {
	std::cout << "Error conn2 F_GETFL\n";
	return 1;
}
retval = fcntl(conn2, F_SETFL, flags | O_NONBLOCK);
if (retval < 0) {
	std::cout << "Error conn2 F_SETFL\n";
	return 1;
}
```

## [ee9bd16] release connection when peer close connection
```cpp
size = recv(conn1, buff, 511, 0);
if (size == 0) {
	std::cout << "conn1 closed gracefully\n";
	break;
}
if (size < 0 && errno == ECONNRESET)
{
	std::cout << "conn1 closed by reset\n";
	break;
}

// ...

// close connection
close(conn1);
close(conn2);
close(sock);

```

## [6589db2] use IO multiplexing to reduce cpu usage

```cpp
// currently there are loops for new connection sockets
// and loops for events on them
```

```cpp
// poll() is now listening events and connection can be closed
// and remade
```

## [ko] use more than two client connections

## [ko] use simple message sending with socket numbers