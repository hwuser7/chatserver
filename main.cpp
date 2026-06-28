#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <poll.h>
#include <vector>

#define PORT 9998

int makeSockNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		std::cout << "Error F_GETFL\n";
		return -1;
	}
	int retval;
	retval = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	if (retval < 0) {
		std::cout << "Error F_SETFL\n";
		return -1;
	}
	return 0;
}

int setSockOpt(int sock) {
	int retval;
	int opt = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (retval < 0) {
		std::cout << "Error setsock\n";
		return -1;
	}
	return 0;
}

int bindSock(int sock) {
	int retval;
	sockaddr_in serv = sockaddr_in();
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (sockaddr *)&serv, sizeof serv);
	if (retval < 0) {
		std::cout << "Error bind\n";
		return -1;
	}
	return 0;
}

int startListeningSock(int *mSock) {
	int retval;
	int sock;

	// main socket
	*mSock = socket(AF_INET, SOCK_STREAM, 0);
	sock = *mSock;
	if (sock < 0) {
		std::cout << "Error socket\n";
		return -1;
	}

	// make socket non-blocking
	retval = makeSockNonBlocking(sock);
	if (retval < 0) 
		return -1;

	// set socket option
	retval = setSockOpt(sock);
	if (retval < 0)
		return -1;

	// bind socket
	retval = bindSock(sock);
	if (retval < 0) {
		return -1;
	}

	// listen
	retval = listen(sock, 4096);
	if (retval < 0) {
		std::cout << "Error listen\n";
		return -1;
	}
	return 0;
}

void 

void checkRevents(int ev, std::vector<pollfd> fds) {
	int i;
	int size;
	int retval;
	pollfd temp;

	retval = 0;
	i = 0;
	size = fds.size();
	while (i < size) {
		if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
			if (fds[i].revents & (POLLERR | POLLHUP)) {
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
				--i;
				--size;
			}
			std::cout << "Error events\n";
		}
		if (i == 0 && (fds[i].revents & POLLIN)) {
			retval = accept(fds[0], NULL, NULL);
			temp.fds = retval;
			temp.events = POLLIN;
			temp.revent = 0;
			
			while (retval > -1) {
				retval = accept(fds[0], NULL, NULL);
				temp.fds = retval;
				temp.events = POLLIN;
				temp.revent = 0;
			}
		}
		// check events on clients
		i++;
	}
}

int main() {
	int retval;
	int sock;

	// start listening master socket
	retval = startListeningSock(&sock);
	if (retval < 0)
		return 1;

	// start polling
	pollfd mainSocket;
	mainSocket.fd = sock;
	mainSocket.events = POLLIN;
	mainSocket.revents = 0;

	std::vector<pollfd> fds;
	fds.push_back(mainSocket);

	retval = poll(&fds[0], fds.size(), -1);

	if (retval < 0)
		std::cout << "Error poll\n";
	if (retval > 0) {
		checkRevents(retval, fds);
	}

	return 0;
}
