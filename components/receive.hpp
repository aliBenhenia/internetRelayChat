#ifndef RECEIVE_HPP
#define RECEIVE_HPP

#pragma once

#include <split.hpp>
#include <to_string.hpp>
#include <defines.hpp>
#include <sys/socket.h>

inline std::string	receive( int socket ) {
	int16_t							bytesRead;
	char								buffer[RECV_MAX];

	std::string result;

	if (socket < 0)
		throw std::runtime_error("invalid socket");

	bytesRead = 0;
	buffer[0] = '\0';
	bytesRead = (int16_t)recv(socket, buffer, RECV_MAX - 1, 0);
	if (bytesRead == 0)
		throw std::runtime_error("offline");
	else if (bytesRead == error && errno != EAGAIN && errno != EWOULDBLOCK)
		throw std::runtime_error("recv function failed: " + to_string(errno == EAGAIN ? "EAGAIN" : "OTHER ERROR"));

	if (bytesRead >= 0)
		buffer[bytesRead] = '\0';
	return to_string(buffer);
}


#endif /* RECEIVE_HPP */