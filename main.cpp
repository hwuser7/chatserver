#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9998

int main() {
	// main socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cout << "Error socket\n";
		return 1;
	}
	int opt = 1;
	int retval;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (retval < 0) {
		std::cout << "Error setsock\n";
		return 2;
	}

	// bind
	sockaddr_in serv = sockaddr_in();
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (sockaddr *)&serv, sizeof serv);
	if (retval < 0) {
		std::cout << "Error bind\n";
		return 3;
	}

	// listen
	retval = listen(sock, 4096);
	if (retval < 0) {
		std::cout << "Error listen\n";
		return 4;
	}

	// accept
	int conn1 = accept(sock, NULL, NULL);
	if (conn1 < 0) {
		std::cout << "Error accept\n";
		return 5;
	}
	
	// have conn1
	int conn2 = accept(sock, NULL, NULL);
	if (conn2 < 0) {
		std::cout << "Error accept\n";
		return 6;
	}

	// have conn2


	char buff[512];
	ssize_t size;

	while (1) {
		size = recv(conn1, buff, 511, 0);
		if (size > 0) {
			buff[size] = '\0';
			size = send(conn2, buff, size, 0);
			if (size < 0) {
				std::cout << "Error send\n";
				return 7;
			}
		}
		size = recv(conn2, buff, 511, 0);
		if (size > 0) {
			buff[size] = '\0';
			size = send(conn1, buff, size, 0);
			if (size < 0) {
				std::cout << "Error send\n";
				return 8;
			}
		}
	}

	return 0;
}