#include <server.hpp>
static std::string getAllMsg(std::vector<std::string> cmd)
{
	std::string msg;
	size_t i = 3;

	while (i < cmd.size())
	{
		if (i == 3 && cmd.at(i)[0] == ':')
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

void Server::kickClient(Channel *channel_, std::string NickName, int clientSocket, Client &_client, std::string reason)
{
	std::string	res;

	if (!channel_->hasClient(NickName))
	{
		std::string msg = ":ft_irc 441 " + _client.getNickname() + " " + NickName + " " + channel_->getName() + " :They aren't on that channel" + "\r\n";
		psend(clientSocket, msg.c_str(), msg.size(), 0);
		return;
	}
	if (_client.getNickname() == NickName)
	{
		std::string msg = ":ft_irc 482 " + _client.getNickname() + " " + channel_->getName() + " :You must be a channel half-operator" + "\r\n";
		psend(clientSocket, msg.c_str(), msg.size(), 0);
		return ;
	}
	res = ":" + _client.getNickname() + " KICK " + channel_->getName() + " " + NickName + " :"+ reason + "\r\n";
	channel_->broadcastToMembers(res, -1);
	channel_->removeClient(NickName);
	channel_->removeOperator(NickName);
	channel_->removeInvited(NickName);
	removeChannelInClient(channel_->getName(), NickName);
}

void Server::kickCmd(std::vector<std::string> cmd, int clientSocket, Client &_client)
{
	std::string res;
	std::string msg;
	Channel		*channel_;

   if (cmd.size() < 3)
   {
		std::string msg = ":ft_irc 461 " + _client.getNickname() + " KICK :Not enough parameters" + "\r\n";
		psend(clientSocket, msg.c_str(), msg.size(), 0);
		return;
	}
	channel_ = this->findChannel(cmd.at(1));
	if (channel_)
	{
		if (channel_->hasClient(clientSocket))
		{
			if (channel_->hasOperator(clientSocket))
			{
				if (cmd.size() > 3)
					msg = getAllMsg(cmd);
				if (cmd.at(2).find(',') != std::string::npos)
				{
					std::vector<std::string>parseClients = split(cmd.at(2), ",");
					if (parseClients.size() > 1)
					{           
						size_t i = 0;
						while (i < parseClients.size())
						{
							kickClient(channel_, parseClients.at(i), clientSocket,_client, msg);
							i++;
						}
					}
					else
						kickClient(channel_, parseClients.at(0), clientSocket,_client, msg);
				}
				else
					kickClient(channel_, cmd.at(2), clientSocket,_client, msg);
					return;
			}
			else
			{
				std::string msg = ":ft_irc 482 " + _client.getNickname() + " " + cmd.at(1) + " :You're not channel operator" + "\r\n";
				psend(clientSocket, msg.c_str(), msg.size(), 0);
			}
		}
		else
		{
			std::string msg = ":ft_irc 442 " + _client.getNickname() + " " + cmd.at(1) + " :You're not on that channel" + "\r\n";
			psend(clientSocket, msg.c_str(), msg.size(), 0);
		}
	}
	else
	{
		std::string msg = ":ft_irc 403 " + _client.getNickname() + " " + cmd.at(1) + " :No such channel" + "\r\n";
		psend(clientSocket, msg.c_str(), msg.size(), 0);
	}
}

