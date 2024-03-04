#ifndef POLLFDS_HPP
#define POLLFDS_HPP

#pragma once

#include <cstring> // for memset
#include <utility> // for std::pair
#include <map> // for std::map
#include <poll.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>

typedef int SOCKET, IP;
#define FD_MAX 10240
#define NONE -1

struct clientUtils {
	clientUtils() {
		this->enableOpt = true;
		this->clientAddrLength = sizeof(this->clientAddr);
		memset(&this->clientAddr, 0, sizeof(this->clientAddr));
		memset(this->host, 0, NI_MAXHOST);
		memset(this->service, 0, NI_MAXSERV);
	}

	struct sockaddr_in			clientAddr;
	socklen_t						clientAddrLength;
	char								host[NI_MAXHOST];
	char								service[NI_MAXSERV];
	int								enableOpt;
	int								clientSocket;
};

struct pollfds {
		pollfds( void ) : nfds(0), index(0) {
			memset(this->fds, 0, FD_SETSIZE * sizeof(struct pollfd));
			memset(this->cash, -1, FD_MAX);
			memset(this->registered, -1, FD_MAX);
			memset(this->clientsInfo, -1, FD_MAX);
			memset(this->startTime, -1, FD_MAX);
		}

		int	size() const {
			return static_cast<int>(this->nfds);
		}

		struct pollfd*	getFds( void ) {
			return this->fds;
		}

		long	getStartTime( int socket ) const {
			return startTime[socket];
		};

		inline int	next( void ) {
			if (this->index == this->nfds)
				this->index = 0;
			while (this->index < this->nfds) {
				if ((this->fds[this->index].revents & POLLIN) == POLLIN)
					return this->fds[this->index].fd;
				this->index++;
			}
			// Return -1 indicates that there is no client that needs service.
			return NONE;
		}

		inline bool	event( int index ) {
			if (index >= 0 && index < this->nfds)
				return (this->fds[index].revents & POLLIN) == POLLIN ? true : false;
			// Return -1 indicates that there is no client that needs service.
			return false;
		}

		inline int	getSocket( int index ) {
			if (index >= 0 && index < this->nfds)
				return this->fds[index].fd;
			return NONE;
		}

		inline int	push_back( int socket ) {
			if (socket >= 0 && socket < FD_MAX && this->nfds == FD_SETSIZE)
				return -1; // Return -1 indicates push_back failed to add a new socket tho to insufficient space

			// Add new socket to the list
			this->fds[this->nfds].fd = socket;
			this->fds[this->nfds].events = POLLIN;
			this->cash[socket] = this->nfds;

			this->startTime[socket] = time(0);

			// Increase the number of working sockets.
			return ++this->nfds;
		}

		inline int	erase( int socket ) {
			if (socket >= 0 && socket < FD_MAX && cash[socket] == -1)
				return -1; // Return -1 indicates erase couldn't find socket

			unsigned short index = cash[socket];

			if (index != this->nfds) {
				std::swap(this->fds[this->nfds - 1], this->fds[index]);
				this->cash[this->fds[index].fd] = index;
				this->cash[socket] = -1;
				this->startTime[socket] = -1;
			}

			this->fds[this->nfds].fd = -1;
			this->registered[socket] = -1;

			close(socket);
			// Decrease the number of working sockets.
			return --this->nfds;
		}

		inline void	clear( void ) {
			while (this->nfds >= 0) {
				close(this->fds[this->nfds].fd);
				this->nfds--;
			}
			memset(this->fds, 0, FD_SETSIZE * sizeof(struct pollfd));
		}

		inline void	setInfo( std::pair<SOCKET, IP> info ) {
			this->clientsInfo[info.first] = info.second;
		}

		inline IP	getInfo( int socket ) const {
			if(socket >= 0 && socket < FD_MAX)
				return this->clientsInfo[socket];
			return -1;
		}

		/** @brief Marks a client as registered */
		inline void	Register( int socket ) {
			if(socket >= 0 && socket < FD_MAX)
				this->registered[socket] = socket;
		}

		/** @brief Finds a registered client */
		inline int	find( int socket )  const{
			if(socket >= 0 && socket < FD_MAX)
				if (this->registered[socket] != -1)
					return socket;
			return -1;
		}

	private:
		int						cash[FD_MAX];
		int						registered[FD_MAX];
		unsigned short			nfds;
		struct pollfd			fds[FD_SETSIZE];
		unsigned short 		index;
		long						startTime[FD_MAX];
		int						clientsInfo[FD_MAX];
};

#endif /* POLLFDS_HPP */