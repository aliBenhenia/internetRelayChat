#include <server.hpp>

static std::string getAllMsg(std::vector<std::string> cmd)
{
	std::string	msg;
	size_t		i = 2;

	while (i < cmd.size())
	{
		if (i == 2 && cmd.at(i)[0] == ':')
		{
			msg += cmd.at(i).substr(1) + " ";
			i++;
		}
		else 
		{
			msg += cmd.at(i) + " ";
			i++;
		}
	}
	return (msg);
}

void Server::sendMessage(std::string target_, int clientSocket, Client _client, std::string message)
{
	std::string	res;

	if (target_[0] == '#')
	{
		Channel *channel_ = this->findChannel(target_);
		if (channel_)
		{
			if (channel_->hasClient(clientSocket))
					channel_->broadcastToMembers(":" + _client.getNickname() + " PRIVMSG " + target_ + " :" + message + "\r\n", clientSocket);
			else
			{
					res = ":ft_irc 442 " + _client.getNickname() + " " + target_ + " :You're not on that channel" + "\r\n";
					psend(clientSocket, res.c_str(), res.size(), 0);
					return;
			}
		}
		else
		{
			res = ":ft_irc 403 " + _client.getNickname() + " " + target_ + " :No such channel" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return ;
		}
	}
	else
	{
		std::vector<Client*>::const_iterator it = this->clients->begin();
		Client *target = NULL;
		while (it != this->clients->end())
		{
			if ((*it)->getNickname() == target_)
			{
					target = *it;
					break;
			}
			it++;
		}
		if (target == NULL)
		{
			res = ":ft_irc 401 " + _client.getNickname() + " " + target_ + " :No such nick" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
		}
		else
		{
			if (_client.getNickname() == target_)
					return;
			res = ":" + _client.getNickname() + " PRIVMSG " + target_ + " :" + message + "\r\n";
			psend(target->getSocketDescriptor(), res.c_str(), res.size(), 0);
		}
	}
}

void Server:: privmsgCmd(std::vector<std::string> cmd, int clientSocket, Client _client)
{
	std::string	res;
	std::string	msg;

	if (cmd.size() < 3)
	{
		res = ":ft_irc 461 " + _client.getNickname() + " PRIVMSG :Not enough parameters" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (cmd.size() >= 3)
	{
		if (cmd.size() > 3 && cmd.at(2)[0] == ':')
			msg = getAllMsg(cmd);
		else
			msg = cmd.at(2);
	}
	if (cmd.at(1).find(',') != std::string::npos)
	{
		std::vector<std::string>parsedTargets = split(cmd.at(1), ",");
		if (parsedTargets.size() > 1)
		{               
			size_t i = 0;
			while (i < parsedTargets.size())
			{
				sendMessage(parsedTargets[i], clientSocket, _client, msg);
				i++;
			}
		}
		else
			sendMessage(parsedTargets[0], clientSocket, _client, msg);
		return ;
	}
	sendMessage(cmd.at(1), clientSocket, _client, msg);
}
