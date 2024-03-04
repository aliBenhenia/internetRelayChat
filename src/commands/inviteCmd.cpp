#include <server.hpp>

void Server::inviteCmd(std::vector<std::string> cmd, int clientSocket, Client _client)
{
	if (cmd.size() < 3)
	{
		std::string res = ":ft_irc 461 " + _client.getNickname() + " INVITE :Not enough parameters" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	Channel *channel_ = this->findChannel(cmd.at(2));
	if (channel_)
	{
		if (channel_->hasClient(clientSocket) == 0)
		{
			std::string res = ":ft_irc 442 " + _client.getNickname() + " " + cmd.at(2) + " :You're not on that channel" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return;
		}
		if (channel_->hasOperator(clientSocket) == 0 && channel_->isInviteOnly())
		{
			std::string res = ":ft_irc 482 " + _client.getNickname() + " " + cmd.at(2) + " :You're not channel operator" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return;
		}
		Client *invited = this->findClient(cmd.at(1));
		if (invited)
		{
			if (channel_->hasClient(invited->getSocketDescriptor()))
			{
				std::string res = ":ft_irc 443 " + _client.getNickname() + " " + cmd.at(1) + " " + cmd.at(2) + " :is already on channel" + "\r\n";
				psend(clientSocket, res.c_str(), res.size(), 0);
				return ;
			}
			std::string res = ":" + _client.getNickname() + " INVITE " + cmd.at(1) + " " + cmd.at(2) + "\r\n";
			psend(invited->getSocketDescriptor(), res.c_str(), res.size(), 0);
			std::string res2 = ":ft_irc 341 " + _client.getNickname() + " " + cmd.at(1) + " " + cmd.at(2) + " :Invited successfully" + "\r\n";
			psend(clientSocket, res2.c_str(), res2.size(), 0);
			channel_->addToInvitedList(*invited);
		}
		else
		{
			std::string res = ":ft_irc 401 " + _client.getNickname() + " " + cmd.at(1) + " :No such nick" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
		}
	}
	else
	{
		std::string res = ":ft_irc 403 " + _client.getNickname() + " " + cmd.at(2) + " :No such channel" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
}
