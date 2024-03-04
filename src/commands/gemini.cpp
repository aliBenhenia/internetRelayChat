#include <server.hpp>

void	sendMsg( int socket, std::string message ) {
	psend(socket, message.c_str(), message.size(), 0);
}

void	Server::gemini( int socket, std::string command ) {
	puts("gemini called");
	if (this->geminiSocket == NONE) {
		try {
			sendMsg(socket, to_string(":gemini PRIVMSG ") + this->getClient(socket).getNickname() + " :GEMINI server is down, please try later!\r\n");
		} catch (const std::exception& e) {
			std::cerr << Red << e.what() << Clear << std::endl ;
		}
		throw std::runtime_error("GEMINI server is not connected");
	}
	std::string	message = to_string(socket) + ": " + command + "\r\n";
	std::cout << Blue << "gemini message send--: " << message << Clear << std::endl ;
	sendMsg(this->geminiSocket, message.c_str());
}
