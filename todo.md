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



## [ko] make socket file descriptor async
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



## [ko] use errno in cpp
