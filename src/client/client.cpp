#include <string>
#include <iostream>
#include <colors.hpp>
#include <client.hpp>

Client::Client( int socket ) {
	this->socket = socket;
	this->registered = false;
	this->passwordCheck = false;
	this->nicknameCheck = false;
}

Client::Client( const Client& other ) {
	*this = other;
}

Client::~Client() {}

Client& Client::operator=( const Client& other ) {
	if (this != &other) {
		this->socket = other.getSocketDescriptor();
		this->nickname = other.getNickname();
		this->username = other.getUsername();
	}
	return *this;
}

bool	Client::getPasswordCheck( void ) const {
	return this->passwordCheck;
}

bool	Client::getNicknameCheck( void ) const {
	return this->nicknameCheck;
}

int	Client::getSocketDescriptor( void ) const {
	return this->socket;
}

bool	Client::getRegistered( void ) const {
	return this->registered;
}

std::string	Client::getNickname( void ) const {
	return this->nickname;
}

std::string	Client::getUsername( void ) const {
	return this->username;
}

void	Client::setSocket( const int socket ) {
	this->socket = socket;
}

void	Client::setPasswordCheck( void ) {
	this->passwordCheck = true;
}

void	Client::setNicknameCheck( void ) {
	this->nicknameCheck = true;
}

void	Client::setNickname( const std::string& nickname ) {
	this->nickname = nickname;
}

void	Client::setUsername( const std::string& username ) {
	this->username = username;
}

void	Client::beRegistered( void ) {
	this->registered = true;
}

void Client::addChannel(Channel &channel) {
	channels.push_back(channel);
}
void Client::removeChannel(std::string channelName) {
	std::vector<Channel>::iterator it = channels.begin();
	while (it != channels.end())
	{
		if (it->getName() == channelName)
		{
			channels.erase(it);
			return;
		}
		it++;
	}
}

void Client::broadcastToAllChannels(std::string res)
{
	std::vector<Channel>::iterator it = channels.begin();
	while (it != channels.end())
	{
		it->broadcastToMembers(res, -1);
		it++;
	}
}
