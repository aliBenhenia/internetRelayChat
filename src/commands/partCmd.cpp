#include <server.hpp>

static std::string getAllMsg(std::vector<std::string> cmd)
{
	std::string	msg;
	size_t i = 2;

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

void Server::partChannel(std::string channelName, int clientSocket, Client _client, std::string reason)
{
	std::string	res;
	Channel		*channel_ = this->findChannel(channelName);
	std::string	nickName = _client.getNickname();
	
	if (channel_)
	{
		if (channel_->hasClient(clientSocket))
		{
			if (reason.size() != 0)
					res = ":" + nickName + " PART " + channelName + " :" + reason + "\r\n";
			else
					res = ":" + nickName + " PART " + channelName + "\r\n";
			channel_->broadcastToMembers(res, -1);
			channel_->removeClient(_client);
			channel_->removeOperator(nickName);
			removeChannelInClient(channelName, nickName);
			if (channel_->hasNoClient() == false)
			{
				std::vector<Channel*>::iterator	it;
				it = this->channels->begin();
				while (it != this->channels->end())
				{
					if ((*it) == channel_)
						{
							delete *it;
							channels->erase(it);
							return ;
						}
					it++;
				}
			}
		}
		else
		{
			std::string msg = ":ft_irc 442 " + nickName + " " + channelName + " :You're not on that channel" + "\r\n";
			psend(clientSocket, msg.c_str(), msg.size(), 0);
			return;
		}
	}
	else
	{
		res = ":ft_irc 403 " + nickName + " " + channelName + " :No such channel" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
}

void Server::partCmd(std::vector<std::string> cmd, int clientSocket, Client _client)
{
	std::string	res;
	std::string	msg;

	if (cmd.size() < 1 || (cmd.size() == 2 && !cmd[1].compare(":")))
	{
		res = ":ft_irc 461 " + _client.getNickname() + " PRIVMSG :Not enough parameters" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (cmd.size() >= 3)
		msg = getAllMsg(cmd);
	if (cmd.at(1).find(',') != std::string::npos)
	{
		std::vector<std::string>parsedChannels = split(cmd.at(1), ",");
		if (parsedChannels.size() > 1)
	{               
		size_t i = 0;
		while (i < parsedChannels.size())
		{
			partChannel(parsedChannels[i], clientSocket, _client, msg);
					i++;
		}
	}
		else
			partChannel(parsedChannels[0], clientSocket, _client, msg);
		return;
	}
	partChannel(cmd.at(1), clientSocket, _client, msg);
}
