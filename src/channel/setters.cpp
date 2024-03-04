#include <channel.hpp>

void	Channel::setPass( std::string pass ) {
	this->channel_pass = pass;
}

void	Channel::setProtected( bool value ) {
	this->isProtected_ = value;
}

void	Channel::set_inviteChannel( bool value ) {
	this->inviteChannel = value;
}

void	Channel::set_topicProtected( bool value ) {
	this->topicProtected = value;
}

void	Channel::setTopic( std::string topic_ ) {
	this->topic = topic_;
}

void	Channel::set_limit( int limit_ ) {
	this->limit = limit_;
}

void	Channel::setLimitMode( bool val ) {
	this->limited = val;
}

void Channel::setNickClient( int clientSocket, NAME& nickname ) {
	std::vector<Client>::iterator	it = this->clients.begin();

	while (it != this->clients.end()) {
		if (it->getSocketDescriptor() == clientSocket)
			it->setNickname(nickname);
		it++;
	}
}

void	Channel::setNickInvited( int clientSocket, NAME& nickname ) {
	std::vector<Client>::iterator	it = this->invited.begin();

	while (it != this->invited.end()) {
		if (it->getSocketDescriptor() == clientSocket)
			it->setNickname(nickname);
		it++;
	}
}

void	Channel::setNickOperators( int clientSocket, NAME& nickname ) {
	std::vector<Client>::iterator	it = this->operators.begin();

	while (it != this->operators.end()) {
		if (it->getSocketDescriptor() == clientSocket)
			it->setNickname(nickname);
		it++;
	}
}
