#ifndef SENDMESSAGE
#define SENDMESSAGE

#pragma once

#include <sys/socket.h>
#include <exception>
#include <colors.hpp>
#include <iostream>
#include <unistd.h>

inline int	psend( int socket, const char* message, size_t size, int x ) {
	if (socket >= 0) {
		ssize_t result = send(socket, message, size, x);
		if (result == -1) {
			std::cerr << "Send Client: " << socket << ": " << strerror(errno) << std::endl;
			return -1;
		}
	}
	return 0;
}

#endif