#include <server.hpp>
#include <thread>
#include <chrono>

void	Server::authPass( SOCKET socket, std::string commandString ) {
	std::string	command;
	std::string	params;

	size_t		space;

	space = commandString.find(' ');
	if (space == std::string::npos || space == 0 || space == commandString.size() - 1) {
		sendErrorMessage(socket, ERR_NEEDMOREPARAMS, NICK_NA, "No Password given");
		throw std::runtime_error("No nickname given");
	}

	command = commandString.substr(0, commandString.find(' '));
	params  = commandString.substr(commandString.find(' ') + 1, std::string::npos);

	try {
		if (this->getClient(socket).getPasswordCheck() == true) {
			std::cerr << Red << "Duplicate PASS command" << Clear << std::endl ;
			sendErrorMessage(socket, "Duplicate PASS command");
			throw std::runtime_error("Duplicate PASS command");
		}
	} catch (...) { std::cerr << Red << "No client found" << Clear << std::endl ; }
	if (params.empty()) {
		std::cerr << Red << "No password given" << Clear << std::endl ;
		sendErrorMessage(socket, ERR_NEEDMOREPARAMS, NICK_NA, "No password given");
		throw std::runtime_error("No password given");
	}
	if (this->password != params) {
		std::cerr << Red << "Password incorrect" << Clear << std::endl ;
		sendErrorMessage(socket, ERR_PASSWDMISMATCH, NICK_NA, "Password incorrect");
		throw std::runtime_error("Password incorrect");
	}
	Client*	client = new Client(socket);
	client->setPasswordCheck();
	this->clients->push_back(client);
}

void	Server::authNick( SOCKET socket, std::string commandString ) {
	std::string	command;
	std::string	params;
	size_t		space;

	space = commandString.find(' ');
	if (space == std::string::npos || space == 0 || space == commandString.size() - 1) {
		sendErrorMessage(socket, ERR_NONICKNAMEGIVEN, NICK_NA, "No nickname given");
		throw std::runtime_error("No nickname given");
	}
	command = commandString.substr(0, space);
	params  = commandString.substr(space + 1, std::string::npos);

	std::cout << "authNick: |" << commandString << "|" << std::endl ;

	try {
		if (this->getClient(socket).getNicknameCheck() == true) {
			sendErrorMessage(socket, "Duplicate NICK command");
			throw std::runtime_error("Duplicate NICK command");
		}
	} catch (const std::exception& e) {
		std::cerr << Red << e.what() << Clear << std::endl ;
		sendErrorMessage(socket, "NICK command is not in the correct order, must be proceeded by PASS");
		throw std::runtime_error("NICK command is not in the correct order, must be proceeded by PASS");
	}
	if (params.empty()) {
		sendErrorMessage(socket, ERR_NONICKNAMEGIVEN, NICK_NA, "No nickname given");
		throw std::runtime_error("No nickname given");
	}
	if (this->findClient(params) != NULL) {
		sendErrorMessage(socket, ERR_NICKNAMEINUSE, NICK_NA, "Nickname " + params + " is already in use");
		throw std::runtime_error("Nickname " + params + " is already in use");
	}
	if (validateNickname(params) != true) {
		sendErrorMessage(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Nickname " + params + " is not a valid nickname");
		throw std::runtime_error("Nickname " + params + " is not a valid nickname");
	}
	try {
		this->getClient(socket).setNickname(params);
		this->getClient(socket).setNicknameCheck();
	} catch (const std::exception& e) {
		std::cerr << Red << e.what() << Clear << std::endl ;
	}
}

std::string	extract( std::string& params ) {
	size_t	pos = params.find_first_of(':');

	(pos == std::string::npos && (pos = params.find_last_of(' ')));
	return pos != std::string::npos ? params.substr(pos + 1) : "";
}

void	Server::authUser( SOCKET socket, IP ip, std::string commandString ) {
	std::string												command;
	std::vector<std::string>							params;
	std::string												fullname;

	command	= commandString.substr(0, commandString.find(' '));
	params  = split(commandString.substr(commandString.find(' ') + 1, std::string::npos), " ");

	try {
		if (this->getClient(socket).getNicknameCheck() != true) {
			sendErrorMessage(socket, "USER command is not in the correct order, must be proceeded by NICK");
			throw std::runtime_error("USER command is not in the correct order, must be proceeded by NICK");
		}
	} catch (const std::exception& e) {
		std::cerr << Red << e.what() << Clear << std::endl ;
		sendErrorMessage(socket, "USER command is not in the correct order, must be proceeded by NICK");
		throw std::runtime_error("USER command is not in the correct order, must be proceeded by NICK");
	}
	fullname = extract(commandString);
	if (params.size() < 3) {
		sendErrorMessage(socket, ERR_NEEDMOREPARAMS, NICK_NA, "Usage: USER <username> <unused> <unused> :<realname>");
		throw std::runtime_error("Usage: USER <username> <unused> <unused> :<realname>");
	}
	// TODO: Check with pears if this is the correct way to validate the username, or I shouldn't validate it at all
	std::cout << "params[0]: |" << params[0] << "|" << std::endl ;
	if (validateNickname(params[0]) != true) {
		sendErrorMessage(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Username " + params[0] + " is not valid");
		throw std::runtime_error("Username " + params[0] + " is not valid");
	}
	std::cout << "fullname: |" << fullname << "|" << std::endl ;
	if (fullname.empty() && validateFullname(fullname) != true) {
		sendErrorMessage(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Realname " + fullname + " is not valid");
		throw std::runtime_error("Realname " + fullname + " is not valid");
	}
	try {
		Client&	client = this->getClient(socket);
		client.beRegistered();
		if (client.getNickname() == "gemini") {
			this->geminiSocket = socket;
			std::cout << Yellow << "GEMINI client registered: " << socket << Clear << std::endl ;
		}
		this->fds.Register(socket);
		sendWelcomeMessage(socket, ip, params[0], client.getNickname());
	} catch (const std::exception& e) {
		std::cerr << Red << e.what() << Clear << std::endl ;
	}
}