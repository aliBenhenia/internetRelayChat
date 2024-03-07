#include <server.hpp>


void Server::joinChannel(std::string channelName, std::string key, int clientSocket, Client &_client)
{
	std::string res;
	Channel		*channel_ = this->findChannel(channelName);

	if (channel_)
	{
		if (channel_->hasClient(clientSocket))
		{
			res = ":ft_irc 479 " + _client.getNickname() + " " + channelName + " :Cannot join channel (already a member)" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return;
		}
		if (channel_->isProtected())
		{
			if (key.empty())
			{
				res = ":ft_irc 475 " + _client.getNickname() + " " + channelName + " :Key required for this channel" + "\r\n";
				psend(clientSocket, res.c_str(), res.size(), 0);
				return;
			}
			if (key == channel_->getPass())
			{
				if (channel_->isLimitMode() && channel_->getClientSize() >=  channel_->getLimit())
				{
					res = ":ft_irc 471 " + _client.getNickname() + " " + channelName + " :Cannot join channel (+l)" + "\r\n";
					psend(clientSocket, res.c_str(), res.size(), 0);
					return;
				}
				if (channel_->isInviteOnly())
				{
					if (channel_->hasInvited(_client.getNickname()) == 0)
					{
						res = ":ft_irc 473 " + _client.getNickname() + " " + channelName + " :Cannot join channel (+i)" + "\r\n";
						psend(clientSocket, res.c_str(), res.size(), 0);
						return;
					}
				}
				channel_->addClient(_client);
				_client.addChannel(*channel_);
				res = ":" + _client.getNickname() + "!" + _client.getUsername() + "@" + this->ipAddresses.at(clientSocket) + " JOIN " + channelName + "\r\n";
				channel_->broadcastToMembers(res, -1);
				res = ":" + _client.getNickname() + " 353 " + _client.getNickname() + " = " + channelName + " :" + channel_->getMembers() + "\r\n";
				channel_->broadcastToMembers(res, -1);
				res = ":" + _client.getNickname() + " 366 " + _client.getNickname() + " " + channelName + " :End of /NAMES list" + "\r\n";
				channel_->broadcastToMembers(res, -1);
				return ;
			}
			else
			{
				res = ":ft_irc 475 " + _client.getNickname() + " " + channelName + " :Wrong channel key" + "\r\n";
				psend(clientSocket, res.c_str(), res.size(), 0);
				return;
			}
		}
		if (channel_->isInviteOnly())
		{
			if (channel_->hasInvited(_client.getNickname()) == 0)
			{
				res = ":ft_irc 473 " + _client.getNickname() + " " + channelName + " :Cannot join channel (+i)" + "\r\n";
				psend(clientSocket, res.c_str(), res.size(), 0);
				return;
			}
		}
		if (channel_->isLimitMode() && channel_->getClientSize() >=  channel_->getLimit())
		{
			res = ":ft_irc 471 " + _client.getNickname() + " " + channelName + " :Cannot join channel (+l)" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
		}
		else
		{
			channel_->addClient(_client);
			_client.addChannel(*channel_);	
			res = ":" + _client.getNickname() + "!" + _client.getUsername() + "@" + this->ipAddresses.at(clientSocket) + " JOIN " + channelName + "\r\n";
			channel_->broadcastToMembers(res, -1);
			res = ":" + _client.getNickname() + " 353 " + _client.getNickname() + " = " + channelName + " :" + channel_->getMembers() + "\r\n";
			channel_->broadcastToMembers(res, -1);
			res = ":" + _client.getNickname() + " 366 " + _client.getNickname() + " " + channelName + " :End of /NAMES list" + "\r\n";
		}
		return;
	}
	if (checkValidName(channelName) == false)
	{
		res = ":ft_irc 476 " + _client.getNickname() + " " + channelName + " :Invalid channel name\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
	else
	{
		Channel *channel = new Channel(channelName);
		channel->addClient(_client);
		_client.addChannel(*channel);
		channel->addOperator(_client);
		if (key.size() != 0)
		{
			channel->setPass(key);
			channel->setProtected(true);
		}
		channels->push_back(channel);
		res = ":" + _client.getNickname() + "!" + _client.getUsername() + "@" + this->ipAddresses.at(clientSocket) + " JOIN " + channelName + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		res = ":" + _client.getNickname() + " MODE " + channelName + " +o " + _client.getNickname() + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
}

void Server:: joinCmd(std::vector<std::string> cmd,int clientSocket, Client &_client)
{
	if (cmd.size() < 2 || (cmd.size() == 2 && !cmd[1].compare("#")))
	{
		std::string res = ":ft_irc 461 " + _client.getNickname() + " JOIN :No name given" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (cmd.at(1).find(',') != std::string::npos)
	{
		std::vector<std::string>parsedChannels = split(cmd.at(1), ",");
		std::vector<std::string>keys;
		if (cmd.size() >= 3 && cmd[2].size() != 0 && cmd[2].find(',') != std::string::npos)
			keys = split(cmd[2], ",");
		if (parsedChannels.size() > 1)
		{
			size_t i = 0;
			while (i < parsedChannels.size())
			{
				if (i == 0 && cmd.size() >= 3 && keys.size() == 0)
					joinChannel(parsedChannels[i], cmd[2], clientSocket,_client);
				else if (cmd.size() >= 3 && keys.size() > i)
					joinChannel(parsedChannels[i], keys[i], clientSocket,_client);
				else
					joinChannel(parsedChannels[i], "", clientSocket,_client);
				i++;
			}
		}
		else
			joinChannel(parsedChannels.at(0), cmd[2], clientSocket,_client);
	}
	else {
		if (cmd.size() >= 3)
			joinChannel(cmd.at(1), cmd.at(2), clientSocket,_client);
		else
			joinChannel(cmd.at(1), "", clientSocket,_client);
	}
}
