#ifndef BOT_HPP
#define BOT_HPP

#include <pollfds.hpp>
#include <colors.hpp>
#include <to_string.hpp>
#include <receive.hpp>
#include <psend.hpp>

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define TCP_NODELAY 0x01
#define error -1

typedef int SOCKET;

class Bot {
	public:
		Bot( void );
		~Bot();

		// Methods
		void			removeClient( int socket );
		void			serveClient( void );
		void			generateResponse( std::string req ) const ;

	private:
		pollfds		fds;
		int			geminiSocket;
		std::map<SOCKET, std::string>	commandsList;
};

#endif