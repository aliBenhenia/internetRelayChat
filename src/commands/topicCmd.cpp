#include <server.hpp>
static std::string getAllMsg(std::vector<std::string> cmd)
{
	std::string msg;
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

void Server::topicCmd(std::vector<std::string> cmd, int clientSocket, Client _client)
{
	std::string	res;

	if (cmd.size() < 2)
	{
		res = ":ft_irc 461 " + _client.getNickname() + " TOPIC :Not enough parameters" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	Channel	*channel_ = this->findChannel(cmd.at(1));
	if (channel_)
	{
		if (channel_->hasClient(clientSocket))
		{
			if (cmd.size() >= 3)
			{
				if (channel_->isTopicProtected() && !channel_->hasOperator(clientSocket) )
				{
					res = ":ft_irc 482 " + _client.getNickname() + " " + cmd.at(1) + " :You're not channel operator" + "\r\n";
					psend(clientSocket, res.c_str(), res.size(), 0);
					return;
				}
				std::string msg;
				if (cmd.size() > 3)
				{
					msg = getAllMsg(cmd);
				}
				else
					msg = cmd.at(2);
				res = ":" + _client.getNickname() + " TOPIC " + cmd.at(1) + " :" + msg + "\r\n";
				channel_->setTopic(msg);
				channel_->broadcastToMembers(res, -1);
				return ;
			}
			else
			{
				res = ":ft_irc 332 " + _client.getNickname() + " " + cmd.at(1) + " :" + channel_->getTopic() + "\r\n";
				psend(clientSocket, res.c_str(), res.size(), 0);
			}
		}
		else
		{
			res = ":ft_irc 442 " + _client.getNickname() + " " + cmd.at(1) + " :You're not on that channel" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return;
		}
	}
	else
	{
		res = ":ft_irc 403 " + _client.getNickname() + " " + cmd.at(1) + " :No such channel" + "\r\n";
		send(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
}
