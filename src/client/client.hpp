#ifndef CLIENT_HPP
#define CLIENT_HPP

#pragma once

// #include <channel.hpp>
#include <string>
#include <channel.hpp>
#include <vector> 
class Channel;

class Client {
	public:
		Client( int socket );
		Client( const Client& other );
		// Client( int socketDescriptor, const std::string& nickname );
		~Client();

		Client& operator=(const Client& rhs);

		int				getSocketDescriptor		( void ) const;
		bool				getPasswordCheck			( void ) const;
		bool				getNicknameCheck			( void ) const;
		bool				getRegistered				( void ) const;
		std::string			getNickname					( void ) const;
		std::string			getUsername					( void ) const;

		void				setSocket					( const int socket );
		void				setNickname					( const std::string& nickname );
		void				setUsername					( const std::string& username );
		void				setPasswordCheck			( void );
		void				setNicknameCheck			( void );
		void				beRegistered				( void );

		void				addChannel					( Channel &channel );
		void				removeChannel				( std::string channelName );
		void				broadcastToAllChannels	( std::string res );

	private:
		int						socket;
		std::string 			nickname;
		std::string 			username;
		bool						passwordCheck;
		bool						nicknameCheck;
		bool						registered;
		std::vector<Channel>	channels;
};

#endif