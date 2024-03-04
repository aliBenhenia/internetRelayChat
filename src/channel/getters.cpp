#include <channel.hpp>

std::string	Channel::getName( void ) const {
	return this->name;
}

const std::string	Channel::getPass( void ) const {
	return this->channel_pass;
}

std::string	Channel::getTopic( void ) const {
	return this->topic;
}

std::string	Channel::getModes( void ) const {
	std::string res = "+";

	if (this->inviteChannel)
		res += "i";
	if (this->topicProtected)
		res += "t";
	if (this->limit > 0)
		res += "l";
	if (this->isProtected_)
		res += "k";
	return res;
}

size_t	Channel::getLimit( void ) const {
	return this->limit;
}

size_t	Channel::getClientSize( void ) const {
	return this->clients.size();
}

const std::vector<Client>&	Channel::getClients( void ) const {
	return this->clients;
}

const std::vector<Client>&	Channel::getOperators( void ) const {
	return this->operators;
}