#include <server.hpp>
#include <validate-nickname.hpp>

void  Server::setNickInChannels(int clientSocket, std::string nick)
{
	std::vector<Channel*>::iterator	it;

	it = this->channels->begin();
	while (it != this->channels->end())
	{
		if ((*it)->hasClient(clientSocket))
		{
			(*it)->setNickClient(clientSocket, nick);
			(*it)->setNickInvited(clientSocket, nick);
			(*it)->setNickOperators(clientSocket, nick);
		}
		it++;
	}
}

void Server::nickCmd(std::vector<std::string> cmd, int clientSocket, Client &_client)
{
	std::string	res;

	if (cmd.size() < 2 || (cmd.size() == 2 && !cmd[1].compare(":")))
	{
		res = ":ft_irc 431 " + _client.getNickname() + " :No nickname given" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (cmd.at(1).size() > 10)
	{
		res = ":ft_irc 432 " + _client.getNickname() + " " + cmd.at(1) + " :Erroneous nickname" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	std::vector<Client*>::const_iterator it = this->clients->begin();
	while (it != this->clients->end())
	{
		if ((*it)->getNickname() == cmd.at(1))
		{
			res = ":ft_irc 433 " + _client.getNickname() + " " + cmd.at(1) + " :Nickname is already in use" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);;
			return;
		}
		it++;
	}
	if (validateNickname(cmd.at(1)))
	{
		res = ":" + _client.getNickname() + " NICK " + cmd.at(1) + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		std::vector<Channel*>::iterator	it;
		it = this->channels->begin();
		while (it != this->channels->end())
		{
			if ((*it)->hasClient(clientSocket))
			{
				(*it)->broadcastToMembers(res, -1);
			}
			it++;
		}
		_client.setNickname(cmd.at(1));
		setNickInChannels(clientSocket ,cmd.at(1));
	}
	else
	{
		res = ":ft_irc 432 " + _client.getNickname() + " " + cmd.at(1) + " :Erroneous nickname" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
}