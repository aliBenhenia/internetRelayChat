#include "gemini.hpp"
#include <send-message.hpp>

inline double	stime(long past, long present) {
	long difference = present - past;
	double hours = difference / 3600.0;
	return hours;
}

Bot::Bot( void ) {
	sockaddr_in	geminiAddr;
	int			enableOpt = true;

	this->geminiSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->geminiSocket == error)
		throw std::runtime_error("Error creating socket for gemini client");
	geminiAddr.sin_family = AF_INET;
	geminiAddr.sin_port = htons(6667);
	if (inet_pton(AF_INET, "127.0.0.1", &geminiAddr.sin_addr) <= 0)
		throw std::runtime_error("Error converting gemini server address");
	if (connect(this->geminiSocket, reinterpret_cast<sockaddr*>(&geminiAddr), sizeof(geminiAddr)) == error)
		throw std::runtime_error("Error connecting to gemini server");
	if (setsockopt(this->geminiSocket, SOL_SOCKET, SO_REUSEADDR, &enableOpt, sizeof(int)) == error)
		throw std::runtime_error("Set socket option [level: SOL_SOCKET] [option: SO_REUSEADDR] failed :" + to_string(strerror(errno)));
	if (setsockopt(this->geminiSocket, SOL_SOCKET, TCP_NODELAY, &enableOpt, sizeof(int)) == error)
	throw std::runtime_error("Set socket option [level: SOL_SOCKET] [option: TCP_NODELAY] failed :" + to_string(strerror(errno)));
	if (fcntl(this->geminiSocket, F_SETFL, O_NONBLOCK) == error)
		throw std::runtime_error("Error setting gemini socket to non-blocking mode");
	std::string	message = "PASS lol\r\n";
	psend(this->geminiSocket, message.c_str(), message.size(), 0);
	message = "NICK gemini\r\n";
	psend(this->geminiSocket, message.c_str(), message.size(), 0);
	message = "USER GEMINI * 0 :gemini\r\n";
	psend(this->geminiSocket, message.c_str(), message.size(), 0);
	this->fds.push_back(this->geminiSocket);
	std::cout << Green << "GEMINI server socket created at: " << Yellow << this->geminiSocket << Clear << std::endl ;
}

Bot::~Bot() {
	std::cout << "Shutting down the GEMINI server\n" << Green << "See you next time :)" << Clear << std::endl ;
	this->fds.clear();
}

void	Bot::removeClient( int socket ) {
	close(socket);
	this->fds.erase(socket);
}

void	Bot::serveClient( void ) {
	int			clientsToStartServing;
	int			clientId;
	bool			connected = false;
	std::string	req;

	while (true) {
		clientsToStartServing = poll(this->fds.getFds(), this->fds.size(), -1);
		if (clientsToStartServing == error)
			throw std::runtime_error("Polling data from socket in GEMINI failed :" + to_string(strerror(errno)));
		if (clientsToStartServing == 0)
			continue ;
		while (clientsToStartServing--) {
			clientId = this->fds.next();
			if (clientId == NONE)
				continue ;
			try {
				req = receive(clientId);
			} catch (const std::exception& e) {
				if (to_string(e.what()) == "offline")
					std::cerr << Cyan << "Server closed connection.\n" << e.what() << Clear << std::endl ;
				else
					throw std::runtime_error("error: fatal\n" + to_string(e.what()));
				this->removeClient(clientId);
				continue ;
			}

			std::cout << "req: " << req << std::endl ;

			if (connected == false) {
				std::cout << "Connected to GEMINI server" << std::endl ;
				connected = true;
				continue ;
			}

			std::vector<std::string>	commands;
			std::vector<std::string>	tmp;

			if (this->commandsList[clientId].size())
				commands.push_back(this->commandsList[clientId]);

			std::cout << "[" << req[req.size() - 1] << "]" << std::endl ;
			if (req[req.size() - 1] != '\n') {
				this->commandsList[clientId] += req;
				continue ;
			} else {
				tmp = split(req, "\n");
				if (tmp.size()) {
					if (commands.size() && commands.back().find('\n') == std::string::npos) {
						commands.back() += tmp[0];
						tmp.erase(tmp.begin());
					}
					for (std::vector<std::string>::const_iterator it = tmp.begin(); it != tmp.end(); it++)
						commands.push_back(*it);
				}
				this->commandsList[clientId].clear();
			}

			for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); it++) {
				size_t	pos = (*it).find('\r');
				if (pos != std::string::npos)
					(*it).erase(pos, std::string::npos);
				std::cout << *it << std::endl ;
			}

			for (std::vector<std::string>::const_iterator it = commands.begin(); it != commands.end(); it++)
				generateResponse(*it);

		}
	}
}

std::string	getSocket( std::string& req ) {
	std::string	socket = req.substr(0, req.find(':'));
	req.erase(0, req.find(':') + 2);
	return socket;
}

// CMD
void	Bot::generateResponse( std::string req ) const {
	std::string	command;
	std::string	socket;
	std::string	message;

	socket = getSocket(req);
	command = req;
	if (command == "hello")
		message = socket + ": Hello world from 1337, ft_irc is in your service";
	else if (command == "date") {
		time_t	now = time(0);
		char*		date = ctime(&now);
		message = socket + ": " + to_string(date);
	}
	else if (command == "ping")
		message = socket + ": pong";
	else
		message = socket + ": Bad request";
	sendMessage(this->geminiSocket, message + "\r\n");
}
