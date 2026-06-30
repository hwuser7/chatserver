#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <poll.h>
#include <vector>
#include <cstring>

#define PORT 9998

struct User {
	int fd;
	int nick;
};

int makeSockNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "Error F_GETFL: " << std::strerror(errno) << "\n";
		return -1;
	}
	int retval;
	retval = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	if (retval < 0) {
		std::cerr << "Error F_SETFL: " << std::strerror(errno) << "\n";
		return -1;
	}
	return 0;
}

int setSockOptReuse(int sock) {
	int retval;
	int opt = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (retval < 0) {
		std::cerr << "Error setsock: " << std::strerror(errno) << "\n";
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
		std::cerr << "Error bind: " << std::strerror(errno) << "\n";
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
		std::cerr << "Error socket: " << std::strerror(errno) << "\n";
		return -1;
	}

	// make socket non-blocking
	retval = makeSockNonBlocking(sock);
	if (retval < 0) {
			close(sock);
			return -1;
	}


	// set socket option
	retval = setSockOptReuse(sock);
	if (retval < 0) {
			close(sock);
			return -1;
	}

	// bind socket
	retval = bindSock(sock);
	if (retval < 0) {
			close(sock);
			return -1;
	}

	// listen
	retval = listen(sock, SOMAXCONN);
	if (retval < 0) {
			std::cerr << "Error listen: " << std::strerror(errno) << "\n";
			close(sock);
			return -1;
	}
	return 0;
}

int acceptConnection(std::vector<pollfd>& fds) {
	pollfd temp;
	int retval;
	
	retval = accept(fds[0].fd, NULL, NULL);
	if (retval < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			std::cerr << "Error accept: " << std::strerror(errno) << "\n";
		return retval;	
	}
	if (makeSockNonBlocking(retval) < 0) {
		close(retval);
		return -1;
	}
	
	temp.fd = retval;
	temp.events = POLLIN;
	temp.revents = 0;
	fds.push_back(temp);
	return retval;
}

// todo
void processClientEvents(int socket) {

	
}

// todo
void createUser() {

}

int checkRevents(std::vector<pollfd>& fds) {
	int i;
	int size;
	int retval;

	retval = 0;
	i = 0;
	size = fds.size();
	while (i < size) {
		if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
			if (fds[i].revents & (POLLERR | POLLHUP)) {
				close(fds[i].fd);
			}
			fds.erase(fds.begin() + i);
			--i;
			--size;
			std::cerr << "Socket error events\n";
			if (i == -1) {
				std::cerr << "Fatal error listening socket\n";
				return -1;	
			}
			continue;
		}
		if (i == 0 && (fds[i].revents & POLLIN)) {
			retval = acceptConnection(fds);
			while (retval > -1) {
				retval = acceptConnection(fds);
			}
			std::cout << "New connections\n";
			createUser();
		}
		if (i != 0 && (fds[i].revents & POLLIN)) {
			std::cout << "New events on client connections\n";
			processClientEvents();
		}
		i++;
	}
	return 0;
}


int startPolling(int sock) {
	pollfd mainSocket;
	mainSocket.fd = sock;
	mainSocket.events = POLLIN;
	mainSocket.revents = 0;

	std::vector<pollfd> fds;
	fds.push_back(mainSocket);
	
	int retval;
	retval = 0;

	while (1) {
		retval = poll(&fds[0], fds.size(), -1);

		if (retval < 0) {
			if (errno == EINTR)
				continue;
			std::cout << "Error poll: " << std::strerror(errno) << "\n";
		}

		if (retval > 0) {
			if (checkRevents(fds) < 0)
				return -1;
		}
	}
	return 0;
}



int main() {
	int retval;
	int sock;

	retval = startListeningSock(&sock);
	if (retval < 0)
		return 1;

	retval = startPolling(sock);
	if (retval < 0)
		return 1;

	close(sock);
	return 0;
}
