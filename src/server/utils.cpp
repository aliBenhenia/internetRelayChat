#include <server.hpp>
#include <thread>
#include <chrono>

void	sendWelcomeMessage(uint16_t clientSocket, int clientIp, const std::string& username, const std::string& nickName) {
	static const std::string serverName = "ft_irc";
	static const std::string version = "1.0";
	static const std::string userModes = "o";
	static const std::string channelModes = "m";

	struct in_addr ipAddr;
	ipAddr.s_addr = clientIp;

	std::string nick = nickName;
	std::string user = username;
	std::string host = std::string(inet_ntoa(ipAddr));

	std::string welcomeMsg = "001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n";
	std::string yourHostMsg = "002 " + nick + " :Your host is " + serverName + ", running version " + version + "\r\n";
	std::string createdMsg = "003 " + nick + " :This server was created at " + "2024-01-24" + "\r\n";
	std::string myInfoMsg = "004 " + nick + " :" + serverName + " " + version + " " + userModes + " " + channelModes + "\r\n";

	std::string fullMessage = welcomeMsg + yourHostMsg + createdMsg + myInfoMsg;
	size_t fullMessageSize = fullMessage.size();

	psend(clientSocket, fullMessage.c_str(), fullMessageSize, 0);
}

void	sendErrorMessage( int socket, int error_code, std::string nickname, std::string message ) {
	std::string	errorMsg = ":ft_irc " + to_string(error_code) + " " + nickname + " :" + message + "\r\n";
	psend(socket, errorMsg.c_str(), errorMsg.size(), 0);
	close(socket);
	std::cerr << errorMsg << std::endl ;
}

void	sendErrorMessageWithoutClosingConnection( int socket, int error_code, std::string nickname, std::string message ) {
	std::string	errorMsg = ":ft_irc " + to_string(error_code) + " " + nickname + " :" + message + "\r\n";
	psend(socket, errorMsg.c_str(), errorMsg.size(), 0);
	std::cerr << errorMsg << std::endl ;
}

void	sendErrorMessage( int socket, std::string message ) {
	std::string	errorMsg = "ERROR :" + message + "\r\n";
	psend(socket, errorMsg.c_str(), errorMsg.size(), 0);
	close(socket);
	std::cerr << errorMsg << std::endl ;
}

void	sendErrorMessageWithoutClosingConnection( int socket, std::string message ) {
	std::string	errorMsg = "ERROR :" + message + "\r\n";
	psend(socket, errorMsg.c_str(), errorMsg.size(), 0);
	std::cerr << errorMsg << std::endl ;
}

Channel*	Server::findChannel( const std::string& channelName ) {
	std::vector<Channel*>::iterator	it;
	it = this->channels->begin();
	while (it != this->channels->end()) {
		if ((*it)->getName() == channelName)
			return *it;
		it++;
	}
	return (NULL);
}

Client&  Server::getClient( int clientSocket ) {
	std::vector<Client*>::const_iterator it = (*clients).begin();
	while (it != (*clients).end()) {
		if ((*it)->getSocketDescriptor() == clientSocket)
    		return *(*it);
		++it;
	}
	throw std::runtime_error("Client not found");
}

Client* Server::findClient( std::string nickname ) {
	std::vector<Client*>::const_iterator it = (*clients).begin();
	while (it != (*clients).end()) {
		if ((*it)->getNickname() == nickname)
			return (*it);
		++it;
	}
	return NULL;
}

void	Server::removeChannelInClient( std::string nickName, std::string channelName ) {
	std::vector<Client*>::const_iterator it = (*clients).begin();
	while (it != (*clients).end()) {
		if ((*it)->getNickname() == nickName)
		{
			(*it)->removeChannel(channelName);
			return ;
		}
		it++;
	}
}

void	Server::executeCommand(std::string commandString, int socket, Client& client ) {
	try {
		std::vector <std::string> commandTokens = split(commandString, " ");
		std::string	command = commandTokens[0];
		if (command == "PONG")
			return ;
		else if (command == "NICK")
			nickCmd(commandTokens, socket,client);
		else if (command == "KICK")
			kickCmd(commandTokens, socket, client);
		else if (command == "INVITE")
			inviteCmd(commandTokens, socket,client);
		else if (command == "JOIN")
			joinCmd(commandTokens, socket, client);
		else if (command == "PART")
			partCmd(commandTokens, socket, client);
		else if (command == "TOPIC")
			topicCmd(commandTokens, socket, client);
		else if (command == "PRIVMSG")
			privmsgCmd(commandTokens, socket, client);
		else if (command == "MODE")
			modeCmd(commandTokens, socket, client);
		else if (command == "QUIT")
			quiteCmd(socket, client);
		else if (command == "USER" || command == "PASS") {
			std::string response = ":ft_irc 462 " + client.getNickname() + " " + command + " :You may not register again" + "\r\n";
			psend(socket, response.c_str(), response.size(), 0);
		}
		else if (command == "GEMINI") {
			if (commandTokens.size() != 2) {
				std::string response = ":gemini PRIVMSG " + client.getNickname() + " :GEMINI only one parameter is allowed (hello, date, or ping)" + "\r\n";
				psend(socket, response.c_str(), response.size(), 0);
				std::cerr << response << std::endl ;
			}
			else
				try { this->gemini(socket, commandTokens[1]); } catch (...) {}
		}
		else {
			std::string response = ":ft_irc 421 " + client.getNickname() + " " + command + " :Unknown command" + "\r\n";
			psend(socket, response.c_str(), response.size(), 0);
			std::cerr << response << std::endl ;
		}
	}
	catch (const std::exception& e) { std::cerr << e.what() << '\n'; }
}

int	getSocket( std::string& req ) {
	int 			socketNumber;
	std::string	socket = req.substr(0, req.find(':'));
	req.erase(0, req.find(':') + 2);
	try {
		socketNumber = std::stoi(socket);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl ;
		return -1;
	}
	return socketNumber;
}
