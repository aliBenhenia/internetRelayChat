#include <server.hpp>
#include <pollfds.hpp>
#include <map>
#include <cstring>
#include <cstdio>

Server::Server(unsigned int ip, int port, std::string password) {
	struct sockaddr_in			serverAddr;
	int								enableOpt;

	if (ip == nohost || ip < anyhost)
		throw std::runtime_error("Invalid IP address");
	if (port <= 1024 || port > 65535)
		port = IRC_PORT;
	if (password.empty())
		throw std::runtime_error("Password is empty");

	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSocket == error)
		throw std::runtime_error("ERROR :Failed to create server socket: " + to_string(strerror(errno)));
	enableOpt = true;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(ip);
	if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &enableOpt, sizeof(int)) == error)
		throw std::runtime_error("Set socket option [level: SOL_SOCKET] [option: SO_REUSEADDR] failed :" + to_string(strerror(errno)));
	fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
	if (bind(this->serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == error)
		throw std::runtime_error("Binding server socket to port [" + to_string(port) + "] failed :" + to_string(strerror(errno)));
	if (listen(this->serverSocket, maxclients) == error)
		throw std::runtime_error("Marking server socket to listen failed :" + to_string(strerror(errno)));
	this->fds.push_back(this->serverSocket);
	std::cout << Green << "Server socket created at: " << Yellow << this->serverSocket << " port: " << port << Clear << std::endl ;

	memset(this->ru, 0, sizeof(receiveUtils) * FD_MAX);
	this->password = password;
	this->geminiSocket = NONE;
	this->clients = new std::vector<Client*>;
	this->channels = new std::vector<Channel*>;
}

Server::~Server() {
	std::cout << Blue << "Closing server socket: " << this->serverSocket << Clear << std::endl ;
	if (close(this->serverSocket) == error) {
		std::cerr << Red << "Failed to close server socket: " << strerror(errno) << Clear << std::endl;
		return ;
	}
	std::cout << Green << "Server socket: " << this->serverSocket << " has been closed successfully" << Clear << std::endl ;
	for (std::vector<Client*>::iterator it = this->clients->begin(); it != this->clients->end(); it++)
		delete *it;
	for (std::vector<Channel*>::iterator it = this->channels->begin(); it != this->channels->end(); it++)
		delete *it;
	delete this->clients;
	delete this->channels;
}

void	Server::start( void ) {
	int				clientsReadyToServer;
	int				clientId;
	int				count = 0;
	std::string		req;

	while (true) {
		if (count++ == 1000) {
			count = 0;
			this->checkClientConnections();
		}
		clientsReadyToServer = poll(this->fds.getFds(), this->fds.size(), 30);
		if (clientsReadyToServer == error)
			throw std::runtime_error("error: fatal\nPoll in IRC failed with status: " + to_string(strerror(errno)));
		if (clientsReadyToServer == 0)
			continue ;
		for (int i = 0; i <= this->fds.size(); i++) {
			if (this->fds.event(i) == true) {
				// here we get next client to server
				clientId = this->fds.getSocket(i);
				if (clientId == NONE)
					continue ;
				// here we accept a client connection request
				if (clientId == this->serverSocket) {
					try {
						this->fds.setInfo(this->acceptClient());
					} catch (const std::exception& e) {
						std::cerr << Red << e.what() << Clear << std::endl ;
					}
					continue ;
				}

				// here we receive data from the socket
				try {
					req = receive(clientId);
				} catch (const std::exception& e) {
					req.clear();
					try { this->quiteCmd(clientId, this->getClient(clientId)); }
					catch(const std::exception& e) {
						std::cerr << "??1-> " <<  e.what() << '\n';
						this->removeClient(clientId);
					}
					if (to_string(e.what()) == "offline")
						std::cerr << Cyan << "Client closed connection." << Clear << std::endl ;
					else
						std::cerr << Cyan << e.what() << Clear << std::endl ;
					continue ;
				}

				if (req.empty())
					continue ;

				std::vector<std::string>	commands;
				std::vector<std::string>	tmp;

				if (this->commandsList[clientId].size())
					commands.push_back(this->commandsList[clientId]);

				if (req.find('\r') == std::string::npos || req.find('\n') == std::string::npos) {
					this->commandsList[clientId] += req;
					continue ;
				} else {
					tmp = split(req, "\r\n");
					if (tmp.size()) {
						if (commands.size()) {
							commands.back() += tmp[0].erase(tmp[0].find_last_not_of(' '), std::string::npos);
							tmp.erase(tmp.begin());
						}
						for (std::vector<std::string>::const_iterator it = tmp.begin(); it != tmp.end(); it++) {
							size_t	lastNoseSpacePos = (*it).find_last_not_of(' ');
							commands.push_back((*it).substr(0, lastNoseSpacePos == std::string::npos ? 0 : lastNoseSpacePos == (*it).size() ? lastNoseSpacePos : lastNoseSpacePos + 1));
						}
					}
					this->commandsList[clientId].clear();
				}

				for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); it++) {
					size_t	pos = (*it).find('\r');
					if (pos != std::string::npos)
						(*it).erase(pos, std::string::npos);
				}

				for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); it++) {
					// here we register the client if not already registered
					if (this->fds.find(clientId) == -1) {
						try {
							this->registerClient(clientId, this->fds.getInfo(clientId), *it);
							continue ;
						} catch (const std::exception& e) {
							std::cerr << Red << e.what() << Clear << std::endl ;
							commands.clear();
							break ;
						}
					}
					if (clientId != this->geminiSocket) {
						try {
							this->executeCommand(*it, clientId, this->getClient(clientId));
						} catch (const std::exception& e) {
							std::cerr << Red << "--==>" << e.what() << Clear << std::endl ;
							try { this->quiteCmd(clientId, this->getClient(clientId)); }
							catch(const std::exception& e) {
								std::cerr << "??3-> " <<  e.what() << '\n';
								this->removeClient(clientId);
							}
							commands.clear();
							break ;
						}
					} else if (clientId == this->geminiSocket) {
						int	socketNumber = getSocket(*it);
						try {
							std::string	response = ":gemini PRIVMSG " + this->getClient(socketNumber).getNickname() + " :" + *it + "\r\n";
							psend(socketNumber, response.c_str(), response.size(), 0);
						} catch (const std::exception& e) {
							std::cerr << Red << e.what() << Clear << std::endl ;
						}
					}
				}
			}
		}
	}
}

std::pair<SOCKET, IP>	Server::acceptClient( void ) {
	struct sockaddr_in			clientAddr;
	socklen_t						clientAddrLength;
	char								host[NI_MAXHOST];
	char								service[NI_MAXSERV];
	int								enableOpt;
	int								clientSocket;

	enableOpt = true;
	clientAddrLength = sizeof(clientAddr);
	memset(&clientAddr, 0, sizeof(clientAddr));
	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);
	clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLength);
	if (clientSocket == error)
		throw std::runtime_error("Client's connection request failed :" + to_string(strerror(errno)));
	if (setsockopt(clientSocket, SOL_SOCKET, TCP_NODELAY, &enableOpt, sizeof(enableOpt)) == error ||
		setsockopt(clientSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&enableOpt, sizeof(enableOpt)) == error ||
		fcntl(clientSocket, F_SETFL, O_NONBLOCK) == error) {
		sendErrorMessage(clientSocket, "Something went wrong, try after a while");
		throw std::runtime_error("Something went wrong:" + to_string(strerror(errno)));
	}
	if (this->fds.size() + 1 > FD_SETSIZE) {
		sendErrorMessage(clientSocket, "Server capacity is full, try after a while");
		throw std::runtime_error("Server capacity is full");
	}
	if (getnameinfo((struct sockaddr *)&clientAddr, clientAddrLength, host, NI_MAXHOST, service, NI_MAXSERV, 0))
		std::cout << Green << "Client connection accepted with [SD: " << clientSocket << "] [host: " << host << "] [port: " << service << "]" << Clear << std::endl ;
	else if (inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST) != NULL)
		std::cout << Green << "Client connection accepted with [SD: " << clientSocket << "] [host: " << host << "] [port: " << ntohs(clientAddr.sin_port) << "]" << Clear << std::endl ;
	this->fds.push_back(clientSocket);
	this->commandsList[clientSocket] = "";
	this->ipAddresses[clientSocket] = host;
	return std::make_pair(clientSocket, clientAddr.sin_addr.s_addr);
}

void	Server::registerClient( SOCKET socket, IP ip, std::string req ) {
	std::string	command = req.substr(0, req.find(' '));

	try {
		if (command == "PONG")
			return ;
		else if (command == "PASS")
			this->authPass(socket, req);
		else if (command == "NICK")
			this->authNick(socket, req);
		else if (command == "USER")
			this->authUser(socket, ip, req);
		else {
			sendErrorMessageWithoutClosingConnection(socket, ERR_UNKNOWNCOMMAND, NICK_NA, command + " unknown command, please register first to have access to server resources");
			throw std::runtime_error("unknown command");
		}
	} catch (const std::exception& e) { throw std::runtime_error(e.what()); }
}

void	Server::removeClient(int socket) {
	std::vector<Client*>::iterator	it;

	this->fds.erase(socket);
	this->commandsList.erase(socket);
	this->ipAddresses.erase(socket);
	if (socket == this->geminiSocket)
		this->geminiSocket = NONE;
	it = this->clients->begin();
	while (it != this->clients->end()) {
		if ((*it)->getSocketDescriptor() == socket) {
			delete *it;
			it = this->clients->erase(it);
			break ;
		}
		else
			it++;
	}
	std::cout << Red << "Client: " << socket << " has been removed" << Clear << std::endl ;
}

void Server::checkClientConnections( void ) {
	std::string	ping = "PING :ft_irc\r\n";
	int	socketsToRemove[FD_MAX];
	int	count = 0;
	int	socket;

	memset(socketsToRemove, -1, sizeof(int) * FD_MAX);
	for (int i = 0 ; i < this->fds.size(); i++) {
		socket = this->fds.getSocket(i);
		if (socket == this->serverSocket || socket == this->geminiSocket)
			continue ;
		if (psend(socket, ping.c_str(), ping.size(), 0) == error) {
			std::cerr << Red << "Failed to send PING to client: " << socket << Clear << std::endl ;
			socketsToRemove[count++] = socket;
		}
	}
	for (int i = 0; i < count; i++)
		this->removeClient(socket);
}
