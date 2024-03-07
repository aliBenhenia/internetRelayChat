#include <server.hpp>
#include <thread>
#include <chrono>

void	Server::authPass( SOCKET socket, std::string commandString ) {
	std::string	command;
	std::string	params;
	size_t		space;

	try {
		if (this->getClient(socket).getPasswordCheck() == true)
			return sendErrorMessageWithoutClosingConnection(socket, "Duplicate PASS command");
	} catch (...) {  }

	space = commandString.find(' ');
	if (space == std::string::npos || space == 0 || space == commandString.size() - 1) {
		return sendErrorMessage(socket, ERR_NEEDMOREPARAMS, NICK_NA, "No Password given");
	}

	command = commandString.substr(0, commandString.find(' '));
	params  = commandString.substr(commandString.find(' ') + 1, std::string::npos);

	if (params.empty())
		return sendErrorMessage(socket, ERR_NEEDMOREPARAMS, NICK_NA, "No password given");
	if (this->password != params)
		return sendErrorMessage(socket, ERR_PASSWDMISMATCH, NICK_NA, "Password incorrect");
	Client*	client = new Client(socket);
	client->setPasswordCheck();
	this->clients->push_back(client);
}

void	Server::authNick( SOCKET socket, std::string commandString ) {
	std::string	command;
	std::string	params;
	size_t		space;

	try {
		if (this->getClient(socket).getNicknameCheck() == true)
			return sendErrorMessageWithoutClosingConnection(socket, "Duplicate NICK command");
	} catch (const std::exception& e) {
		return sendErrorMessageWithoutClosingConnection(socket, "NICK command is not in the correct order, must be proceeded by PASS");
	}

	space = commandString.find(' ');
	if (space == std::string::npos || space == 0 || space == commandString.size() - 1)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_NONICKNAMEGIVEN, NICK_NA, "No nickname given");

	command = commandString.substr(0, space);
	params  = commandString.substr(space + 1, std::string::npos);

	if (params.empty())
		return sendErrorMessageWithoutClosingConnection(socket, ERR_NONICKNAMEGIVEN, NICK_NA, "No nickname given");
	if (this->findClient(params) != NULL)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_NICKNAMEINUSE, NICK_NA, "Nickname " + params + " is already in use");
	if (validateNickname(params) != true)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Nickname " + params + " is not a valid nickname");
	try {
		this->getClient(socket).setNickname(params);
		this->getClient(socket).setNicknameCheck();
	} catch (const std::exception& e) {
		std::cerr << Red << e.what() << Clear << std::endl ;
	}
}

std::string	extract( std::string& params ) {
	size_t	pos = params.find_last_of(':');

	if (pos == std::string::npos)
		pos = params.find_last_of(' ');
	return pos != std::string::npos ? params.substr(pos + 1) : "";
}

void	Server::authUser( SOCKET socket, IP ip, std::string commandString ) {
	std::string												command;
	std::vector<std::string>							params;
	std::string												fullname;

	try {
		if (this->getClient(socket).getNicknameCheck() != true)
			return sendErrorMessageWithoutClosingConnection(socket, "USER command is not in the correct order, must be proceeded by NICK");
	} catch (const std::exception& e) {
		return sendErrorMessageWithoutClosingConnection(socket, "Please give password first with PASS command");
	}

	size_t	space = commandString.find(' ');
	command	= commandString.substr(0, space);
	params  = split(commandString.substr(space + 1, std::string::npos), " ");

	fullname = extract(commandString);
	int	size = params.size();
	if (size < 3 || (size == 5 && commandString.find(':') == std::string::npos) || fullname[0] == ' ' || size > 5)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_NEEDMOREPARAMS, NICK_NA, "Usage: USER <username> <unused> <unused> :<realname>\nrealname: can be anything, but must contain only one space");
	if (validateNickname(params[0]) != true)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Username " + params[0] + " is not valid");
	if (fullname.empty() || validateFullname(fullname) != true)
		return sendErrorMessageWithoutClosingConnection(socket, ERR_ERRONEUSNICKNAME, NICK_NA, "Realname " + fullname + " is not valid");
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