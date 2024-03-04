#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once

#include <client.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <map>
#include <defines.hpp>
#include <psend.hpp>

class Client;

class Channel {
	public:
		Channel	( void );
		Channel	( const std::string& name );
		~Channel	();

		std::string								getName					( void ) const;
		const std::string						getPass					( void ) const;
		std::string								getTopic					( void ) const;
		std::string								getModes					( void ) const;
		size_t									getLimit					( void ) const;
		size_t									getClientSize			( void ) const;
		const std::vector<Client>&			getClients				( void ) const;
		const std::vector<Client>&			getOperators			( void ) const;

		void										setPass					( std::string pass );
		void										setProtected			( bool value );
		void										set_inviteChannel		( bool value );
		void										set_topicProtected	( bool value );
		void										setTopic					( std::string topic_ );
		void										set_limit				( int limit_ ); 
		void										setLimitMode			( bool val );
		void										setNickClient			( int clientSocket, NAME& nickname );
		void										setNickInvited			( int clientSocket, NAME& nickname );
		void										setNickOperators		( int clientSocket, NAME& nickname );

		void										addClient				( Client client_ );
		void										addOperator				( Client client_ );
		void										addOperator				( NAME nickname );
		void										addToInvitedList		( Client &client_ );

		void										removeClient			( Client& client );
		void										removeClient			( NAME nickname );
		void										removeOperator			( NAME nickname );

		bool										hasClient				( int clientSocket ) const;
		bool										hasClient				( NAME nickname ) const;
		bool										hasInvited				( NAME nickname ) const;
		bool										hasOperator				( int clientSocket ) const;
		bool										hasNoClient				( void ) const;

		bool										isProtected				( void ) const;
		bool										isInviteOnly			( void ) const;
		bool										isLimitMode				( void ) const;
		bool										isTopicProtected		( void ) const;

		void										decrementLimit			( void );

		void										broadcastToMembers	( const std::string& message, int senderSocket );
	std::string getMembers() const;
	private:
		bool										inviteChannel;
		bool										isProtected_;
		bool										topicProtected;
		bool										limited;
		size_t									limit;
		std::string								name;
		std::string								channel_pass;
		std::string								topic;
		std::vector<Client>					clients;
		std::vector<Client>					operators;
		std::vector<Client>					invited;
};

#endif