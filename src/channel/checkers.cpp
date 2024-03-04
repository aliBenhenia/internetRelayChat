#include <channel.hpp>

bool	Channel::hasNoClient( void ) const {
	if (this->clients.size()) return true;
	return false;	
}

bool	Channel::hasClient( int clientSocket ) const {
	std::vector<Client>::const_iterator	it = this->clients.begin();

	while (it != this->clients.end()) {
		if (it->getSocketDescriptor() == clientSocket)
			return (true);
		++it;
	}
	return (false);
}

bool	Channel::hasClient( NAME nickname ) const {
	std::vector<Client>::const_iterator	it = this->clients.begin();

	while (it != this->clients.end()) {
		if (it->getNickname() == nickname)
			return (true);
		++it;
	}
	return (false);
}

bool	Channel::hasInvited( NAME  nickname ) const {
	std::vector<Client>::const_iterator	it = this->invited.begin();

	while (it != this->invited.end()) {
		if (it->getNickname() == nickname)
			return (true);
		++it;
	}
	return (false);
}

bool	Channel::hasOperator( int clientSocket ) const {
	std::vector<Client>::const_iterator	it = this->operators.begin();

	while (it != this->operators.end()) {
		if (it->getSocketDescriptor() == clientSocket)
			return (true);
		++it;
	}
	return (false);
}

bool	Channel::isProtected( void ) const {
	return this->isProtected_;
}

bool	Channel::isInviteOnly( void ) const {
	return this->inviteChannel;
}

bool	Channel::isLimitMode( void ) const {
	return this->limited;
}

bool	Channel::isTopicProtected( void ) const {
	return this->topicProtected;
}
