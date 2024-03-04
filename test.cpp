#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>

#define localhost "127.0.0.1"

inline void	add_poll_fd(int socket , struct pollfd* fds, int& nfds) {
	fds[nfds].fd = socket;
	fds[nfds].events = POLLIN;
	nfds++;
}

template <typename T>
inline std::string to_string( const T& value ) {
	std::stringstream	ss;
	ss << value;
	return ss.str();
}

inline void	eraseFd(int socket, struct pollfd* fds, int& nfds) {
	for (int i = 0; i < nfds; i++) {
		if (fds[i].fd == socket) {
			close(fds[i].fd);
			fds[i] = fds[nfds - 1];
			nfds--;
			break ;
		}
	}
}

int main(int ac, char** av) {
	struct pollfd   fds[FD_SETSIZE];
	std::vector<int>	clients;

	if (ac < 4) {
		std::cout << "not enought arguments" << std::endl ;
		std::cout << "usage :<nfds> <port> <passowrd>" << std::endl ;
		return 1;
	}
	int nfds = 0;
	int numOfClients = std::stoi(av[1]);
	int port = std::stoi(av[2]);
	std::string password = std::string(av[3]);
	std::cout << "numOfClients :" << numOfClients << std::endl ;
	std::cout << "port :" << port << std::endl ;
	std::cout << "password :" << password << std::endl ;
	std::string passCommand = "PASS " + password + "\r\n";
	for (int i = 0; i < numOfClients; i++) {
		std::cout << "i: " << i << " | " << "nfds :" << nfds << std::endl ;
		int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSocket == -1) {
			std::cerr << "Error creating socket for client :" << i << std::endl ;
			continue ;
		}
		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (inet_pton(AF_INET, localhost, &serverAddress.sin_addr) <= 0) {
			std::cerr << "Error converting server address\n";
			close(clientSocket);
			continue ;
		}
		// Connect to the server
		if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
			std::cerr << "Error connecting to server\n";
			close(clientSocket);
			return 1;
		}
		add_poll_fd(clientSocket, fds, nfds);
		std::string nickCommand = "NICK client" + to_string(i) + "\r\n"; 
		std::string userCommand = "USER u_client" + to_string(i) + std::string(" 0 * ") + "realname" + to_string(i) + "\r\n";
		std::cout << passCommand << nickCommand << userCommand << std::endl ;
		send(clientSocket, passCommand.c_str(), passCommand.size(), 0);
		send(clientSocket, nickCommand.c_str(), nickCommand.size(), 0);
		send(clientSocket, userCommand.c_str(), userCommand.size(), 0);
		clients.push_back(clientSocket);
		nickCommand.clear();
		userCommand.clear();
	}
	std::string	join = "JOIN #chann\r\n";
	std::string	privmsg = "PRIVMSG #chann hello\r\n";
	std::string	part = "PART #chann\r\n";
	std::string	quit = "QUIT Leaving...\r\n";
	std::vector<int>::const_iterator it = clients.begin();
	std::cout << clients.size() << std::endl ;

	for (int i = 0; i < clients.size(); i++) {
		std::cout << "Joining chann : " << clients[i] << std::endl ;
		send(clients[i], join.c_str(), join.size(), 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		std::cout << "i: " << i << std::endl ;
	}

	for (int i = 0; i < clients.size(); i++) {
		std::cout << "Privmsg chann : " << clients[i] << std::endl ;
		send(clients[i], privmsg.c_str(), privmsg.size(), 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		std::cout << "i: " << i << std::endl ;
	}

	for (int i = 0; i < clients.size(); i++) {
		std::cout << "Part chann : " << clients[i] << std::endl ;
		send(clients[i], part.c_str(), part.size(), 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		std::cout << "i: " << i << std::endl ;
	}

	char buffer[10500];
	int	recvSize = 0;

	for (int i = 0; i < clients.size(); i++) {
		std::cout << "Recv chann : " << clients[i] << std::endl ;
		recvSize = recv(clients[i], buffer, 10499, 0);
		buffer[recvSize] = '\0';
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		std::cout << "i: " << i << " | content: " << buffer << std::endl ;
	}

	for (size_t i = 0; i < clients.size(); i++) {
		std::cout << "Closing client : " << clients[i] << std::endl ;
		send(clients[i], quit.c_str(), quit.size(), 0);
		close(clients[i]);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		std::cout << "i: " << i << std::endl ;
	}
}
