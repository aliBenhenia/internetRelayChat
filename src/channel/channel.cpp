#include <channel.hpp>

Channel::Channel( const std::string& name ) {
	this->isProtected_ = false;
	this->name = name;
	this->inviteChannel = false;
	this->topicProtected = false;
	this->limited = false;
	this->limit = 0;
}

Channel::~Channel() {}

void	Channel::addClient( Client client_ ) {
	this->clients.push_back(client_);
}

void	Channel::addOperator( Client client_ ) {
	this->operators.push_back(client_);
}

void	Channel::removeClient( Client& client ) {
	std::vector<Client>::iterator it = this->clients.begin();

	while (it != clients.end()) {
		if (it->getSocketDescriptor() == client.getSocketDescriptor()) {
			clients.erase(it);
			return;
		}
		++it;
	}
}

void	Channel::removeClient( NAME nickname ) {
	std::vector<Client>::iterator it = this->clients.begin();

	while (it != clients.end()) {
		if (it->getNickname() == nickname) {
			clients.erase(it);
			return;
		}
		it++;
	}
}

void	Channel::addOperator( NAME nickname ) {
	std::vector<Client>::iterator it = this->clients.begin();

	while (it != clients.end()) {
		if (it->getNickname() == nickname) {
			this->operators.push_back(*it);
			return;
		}
		it++;
	}
}

void	Channel::addToInvitedList( Client &client_ ) {
	this->invited.push_back(client_);
}

void	Channel::removeOperator( NAME nickname ) {
	std::vector<Client>::iterator it = this->operators.begin();

	while (it != this->operators.end()) {
		if (it->getNickname() == nickname) {
			this->operators.erase(it);
			return;
		}
		it++;
	}
}

void	Channel::decrementLimit( void ) {
	if (this->limit > 0)	this->limit--;
}

void	Channel::broadcastToMembers(const std::string& message, int senderSocket) {
	int													socket;
	std::vector<Client>::const_iterator	it = this->clients.begin();
	std::vector<Client>::const_iterator	end = this->clients.end();

	if (senderSocket == -1) // Send the message to the sender if senderSocket == -1
		psend(senderSocket, message.c_str(), message.size(), 0);
	while (it != end) {
		socket = it->getSocketDescriptor();
		if (socket != senderSocket)
			psend(socket, message.c_str(), message.size(), 0);
		it++;
	}
}

std::string Channel::getMembers() const
{
	std::string res;
	std::vector<Client>::const_iterator it = this->clients.begin();
	while (it != clients.end())
	{
		if (hasOperator(it->getSocketDescriptor()))
			res += "@" + it->getNickname() + " ";
		else
			res += it->getNickname() + " ";
		it++;
	}
	return res;
}
