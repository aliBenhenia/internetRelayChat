#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once

#include <include_guard.hpp>
#include <validateChannleName.hpp>
#include <validate-nickname.hpp>
#include <pollfds.hpp>
#include <psend.hpp>

class ICommand;

struct receiveUtils {
	std::vector<std::string>	rawData;
};

class Server {
	public:
		Server(unsigned int ip , int port, std::string password);
		~Server();

		void 							start							( void );
		std::pair<SOCKET, IP> 	acceptClient				( void );
		void							removeClient				( int socket );
		void							registerClient				( SOCKET socket, IP ip, std::string req );
		Channel* 					findChannel					( const std::string& channelName );
		Client& 						getClient					( int clientSocket );
		Client*						findClient					( std::string nickname );
		void 							removeChannelInClient	( std::string nickName, std::string channelName );
		void 							setNickInChannels			( int clientSocket, std::string nick );
		void 							joinChannel					( std::string channelName, std::string key, int clientSocket, Client &_client );
		void 							partChannel					( std::string channelName, int clientSocket, Client _client,std::string reason );
		void 							kickClient					( Channel *channel_, std::string NickName, int clientSocket, Client &_client, std::string reason );
		void 							sendMessage					( std::string target, int clientSocket, Client _client, std::string message );

		void							gemini						( int socket, std::string command );
		void							checkClientConnections	( void );

		void							authPass						( SOCKET socket, std::string commandString );
		void							authNick						( SOCKET socket, std::string commandString );
		void							authUser						( SOCKET socket, IP ip, std::string commandString );

		void 							quiteCmd						( int clientSocket, Client _client );
		void 							joinCmd						( std::vector<std::string> cmd,int clientSocket, Client &_client );
		void 							partCmd						( std::vector<std::string> cmd, int clientSocket, Client _client );
		void 							topicCmd						( std::vector<std::string> cmd, int clientSocket, Client _client );
		void 							privmsgCmd					( std::vector<std::string> cmd, int clientSocket, Client _client );
		void 							modeCmd						( std::vector<std::string> cmd, int clientSocket, Client &_client );
		void 							nickCmd						( std::vector<std::string> cmd, int clientSocket, Client &_client );
		void 							userCmd						( std::vector<std::string> cmd, int clientSocket, Client _client );
		void 							kickCmd						( std::vector<std::string> cmd, int clientSocket, Client &_client );
		void 							inviteCmd					( std::vector<std::string> cmd, int clientSocket, Client _client );
		void 							executeCommand				( std::string command, int socket, Client &client );

	private:
		int							serverSocket;
		int							geminiSocket;
		bool							isRunning;
		std::string					password;
		pollfds						fds;
		pollfds						fds_gemini;
		receiveUtils				ru[FD_MAX];
		std::vector<Client*>* 	clients;
		std::vector<Channel*>* 	channels;
		std::map<SOCKET, std::string>	commandsList;
		std::map<SOCKET, std::string>	ipAddresses;
};

void	sendWelcomeMessage(uint16_t clientSocket, int clientIp, const std::string& username, const std::string& nickName);
std::pair<bool, std::string>	DuplicateCommandDetector(std::vector<std::string> rawData);
void	sendErrorMessage( int socket, int error_code, std::string nickname, std::string message );
void	sendErrorMessageWithoutClosingConnection( int socket, int error_code, std::string nickname, std::string message );
void	sendErrorMessage( int socket, std::string message );
void	sendErrorMessageWithoutClosingConnection( int socket, std::string message );
int	getSocket( std::string& req );

#endif