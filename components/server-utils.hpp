#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <defines.hpp>
#include <poll.h>
#include <pollfds.hpp>

inline void	sendWelcomeMessage(uint16_t clientSocket, int clientIp, std::string& username, std::string& nickName) {
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

inline std::pair<bool, std::string>	DuplicateCommandDetector(std::vector<std::string> rawData) {
	std::vector<std::string>::const_iterator	it;
	uint8_t												size;
	std::map<std::string, uint8_t>				commandCount;

	size = rawData.size();
	it = rawData.begin();

	for (uint8_t i = 0; i < size; it++, i++) {
		std::string command = (*it).substr(0, (*it).find(' '));
		commandCount[command]++;
		if (commandCount[command] > 1) {
			return std::make_pair(true, command);
		}
	}
	return std::make_pair(false, "");
}

inline tuple make_tuple(RS_CODE code, CMD cmd, std::string message) {
	return tuple(code, cmd, message);
}

inline void	add_poll_fd(int socket , struct pollfd* fds, int& nfds) {
	fds[nfds].fd = socket;
	fds[nfds].events = POLLIN;
	nfds++;
}

inline int	validateNickname(const std::string nickname) {
	if (!isalpha(nickname[0]) || !std::any_of(nickname.begin(), nickname.end(), isalnum))
		return false;
	return true;
}

inline bool checkValidName(std::string name) {
	std::string tmp = name.substr(0,1);
	if (name.empty() || name[0] != '#' || name.size() > 50)
		return false;
	if (tmp.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#") != std::string::npos)
		return false;
	return true;
}

#endif