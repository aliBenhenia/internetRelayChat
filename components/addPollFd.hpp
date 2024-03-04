#ifndef POLL_FD_HPP
#define POLL_FD_HPP

#pragma once

#include <poll.h>

inline void	add_poll_fd(int socket , struct pollfd* fds, int& nfds) {
	fds[nfds].fd = socket;
	fds[nfds].events = POLLIN;
	nfds++;
}

#endif /* POLL_FD_HPP */