#ifndef SEND_MESSAGE
#define SEND_MESSAGE

#include <sys/socket.h>
#include <string>
#include <psend.hpp>

inline void	sendMessage(int socket, std::string message) {
	psend(socket, message.c_str(), message.size(), 0);
}

#endif