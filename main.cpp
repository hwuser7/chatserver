#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <poll.h>

#define PORT 9998

void makeNonBlocking(int socketFD) {
	int flags = fcntl(socketFD, F_GETFL, 0);
	if (flags < 0) {
		std::cout << "Error F_GETFL\n";
		return ;
	}
	int retval;
	retval = fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
	if (retval < 0) {
		std::cout << "Error F_SETFL\n";
		return ;
	}
}

int main() {
	// main socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cout << "Error socket\n";
		return 1;
	}

	// make it non-blocking
	makeNonBlocking(sock);

	int retval;
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


	// fds size 3 (2 connection sockets and 1 master socket)
	pollfd fds[3];
	
	pollfd master;
	master.fd = sock;
	master.events = POLLIN;
	master.revents = 0;
	
	pollfd conn1;
	conn1.fd = -1;
	conn1.events = 0;
	conn1.revents = 0;

	pollfd conn2;
	conn2.fd = -1;
	conn2.events = 0;
	conn2.revents = 0;

	fds[0] = master;
	fds[1] = conn1;
	fds[2] = conn2;

	char buff[512];

	while (1) {
		retval = poll(fds, 3, -1);
		if (retval < 0) {
			std::cout << "Error poll\n";
		}
		// check if new connection
		if (fds[0].revents & POLLIN) {
			// add new conection to fds
			if (fds[1].fd == -1) {
				fds[1].fd = accept(sock, NULL, NULL);
				makeNonBlocking(fds[1].fd);
				fds[1].events = POLLIN;
				fds[1].revents = 0;
			}
			else if (fds[2].fd == -1) {
				fds[2].fd = accept(sock, NULL, NULL);
				makeNonBlocking(fds[2].fd);
				fds[2].events = POLLIN;
				fds[2].revents = 0;
			}
			else {
				std::cout << "Error connection limit reached\n";
			}
		}
		if (fds[1].fd != -1 && fds[2].fd != -1) {
			if (fds[1].revents & POLLIN) {
				retval = recv(fds[1].fd, buff, 511, 0);
				if (retval < 0) {
					std::cout << "Error recive from socket\n";
				}
				else if (retval == 0) {
					std::cout << "Remote closed connection\n";
					fds[1].fd = -1;
					fds[1].events = 0;
					fds[1].revents = 0;
				}
				else {
					buff[retval] = '\0';
					retval = send(fds[2].fd, buff, retval+1, 0);
					if (retval == 0) {
						std::cout << "Remote closed connection\n";
						fds[2].fd = -1;
						fds[2].events = 0;
						fds[2].revents = 0;
					}
					else if (retval < 0) {
						std::cout << "Error sending to socket\n";
					}
					else {
						std::cout << "Message send\n";
					}
				}

			}
			else if (fds[2].revents & POLLIN) {
				retval = recv(fds[2].fd, buff, 511, 0);
				if (retval < 0) {
					std::cout << "Error recive from socket\n";
				}
				else if (retval == 0) {
					std::cout << "Remote closed connection\n";
					fds[2].fd = -1;
					fds[2].events = 0;
					fds[2].revents = 0;
				}
				else {
					buff[retval] = '\0';
					retval = send(fds[1].fd, buff, retval+1, 0);
					if (retval == 0) {
						std::cout << "Remote closed connection\n";
						fds[1].fd = -1;
						fds[1].events = 0;
						fds[1].revents = 0;
					}
					else if (retval < 0) {
						std::cout << "Error sending to socket\n";
					}
					else {
						std::cout << "Message send\n";
					}
				}

			}
		}
	}

	close(fds[1].fd);
	close(fds[2].fd);
	close(sock);

	return 0;
}
