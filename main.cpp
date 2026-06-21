#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#define PORT 9998

int main() {
	// main socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cout << "Error socket\n";
		return 1;
	}

	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		std::cout << "Error F_GETFL\n";
		return 1;
	}
	int retval;
	retval = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	if (retval < 0) {
		std::cout << "Error F_SETFL\n";
		return 1;
	}


	int opt = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (retval < 0) {
		std::cout << "Error setsock\n";
		return 1;
	}

	// bind
	sockaddr_in serv = sockaddr_in();
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (sockaddr *)&serv, sizeof serv);
	if (retval < 0) {
		std::cout << "Error bind\n";
		return 1;
	}

	// listen
	retval = listen(sock, 4096);
	if (retval < 0) {
		std::cout << "Error listen\n";
		return 1;
	}

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


	char buff[512];
	ssize_t size;

	while (1) {
		size = recv(conn1, buff, 511, 0);
		if (size > 0) {
			buff[size] = '\0';
			size = send(conn2, buff, size, 0);
			if (size < 0) {
				std::cout << "Error send\n";
				return 1;
			}
		}
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

	return 0;
}
