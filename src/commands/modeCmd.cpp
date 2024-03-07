#include <server.hpp>

static bool containOnlyNumbers(std::string s)
{
	int i = 0;
	while (s[i] && isdigit(s[i]))
	i++;
	if (s[i] == '\0')
		return (true);
	return (false);
}

static void notEnoughParamsErr(int clientSocket, std::string nickName)
{
	std::string  res = ":ft_irc 461 " + nickName + " MODE :Not enough parameters" + "\r\n";
	psend(clientSocket, res.c_str(), res.size(), 0);
}


static void unkownCmd(int clientSocket, std::string nickName, std::vector<std::string> cmd, char mode)
{
	std::string  res = ":ft_irc 472 " + nickName + " " + cmd.at(1) + " :is unknown mode char to me for " + mode + "\r\n";
	psend(clientSocket, res.c_str(), res.size(), 0);
}

static void modeResponse(std::string nickName, std::vector<std::string> cmd, Channel *channel_, std::string mode)
{
	std::string res = ":" + nickName + " MODE " + cmd.at(1) + " " + mode + "\r\n";
	channel_->broadcastToMembers(res, -1);
}

static void inviteMode(char modeType , Channel *channel_, std::vector<std::string> cmd,Client &_client)
{
	if (modeType == '+')
	{
		channel_->set_inviteChannel(true);
		modeResponse(_client.getNickname(), cmd, channel_, "+i");
		return;
	}
	channel_->set_inviteChannel(false);
	modeResponse(_client.getNickname(), cmd, channel_, "-i");
}

static void topicMode(char modeType ,Channel *channel_, std::vector<std::string> cmd,Client &_client)
{
	if (modeType == '+')
	{
		channel_->set_topicProtected(true);
		modeResponse(_client.getNickname(), cmd, channel_,"+t");
		return;
	}
	channel_->set_topicProtected(false);
	modeResponse(_client.getNickname(), cmd, channel_, "-t");
}

static void operatorMode(char modeType , std::string user_ ,Channel *channel_,int clientSocket, std::vector<std::string> cmd,Client &_client)
{
	std::string res;
	if (user_.empty())
	{
		res = ":ft_irc 461 " + _client.getNickname() + " MODE :Not enough parameters" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (!channel_->hasClient(user_))
	{
		std::string res = ":ft_irc 401 " + _client.getNickname() + " " + user_ + " :No such nick" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
		return;
	}
	if (modeType == '+')
	{
		
		channel_->addOperator(user_);
		res = ":" + _client.getNickname() + " MODE " + cmd.at(1) + " " +  "+o"  + " " + user_ + "\r\n";
		channel_->broadcastToMembers(res, -1);
		return;
	}
	channel_->removeOperator(user_); 
	res = ":" + _client.getNickname() + " MODE " + cmd.at(1) + " " + "-o" + " " + user_ + "\r\n";
	channel_->broadcastToMembers(res, -1);
}

static void limitMode(char modeType, std::string limit ,Channel *channel_,int clientSocket, std::vector<std::string> cmd,Client &_client)
{
	std::string	res;

	if (modeType == '+')
	{
		if (limit.empty())
		{
			notEnoughParamsErr(clientSocket, _client.getNickname());
			return;
		}
		if (containOnlyNumbers(limit) == false)
		{
			res = ":ft_irc 472 " + _client.getNickname() + " " + limit + " :unknown mode char to me" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return ;
		}
		if (std::atoi(limit.c_str()) < 0)
		{
			res = ":ft_irc 472 " + _client.getNickname() + " " + limit + " :is unknown mode char to me" + "\r\n";
			psend(clientSocket, res.c_str(), res.size(), 0);
			return ;
		}
		channel_->set_limit(std::atoi(limit.c_str()));
		channel_->setLimitMode(true);
		res = ":" + _client.getNickname() + " MODE " + cmd.at(1) + " " + "+l" + " " + limit + "\r\n";
		channel_->broadcastToMembers(res, -1);
		return;
	}
	channel_->setLimitMode(false);
	channel_->set_limit(0);
	modeResponse(_client.getNickname(), cmd, channel_, "-l");
}
static void keyMode(char modeType , std::string pass ,Channel *channel_,int clientSocket, std::vector<std::string> cmd,Client &_client)
{
	std::string	res;

	if (modeType == '+')
	{
		if (pass.empty())
		{
			notEnoughParamsErr(clientSocket, _client.getNickname());
			return;
		}
		channel_->setPass(pass);
		channel_->setProtected(true);
		res = ":" + _client.getNickname() + " MODE " + cmd.at(1) + " " + "+k" + " "  + "\r\n";
		channel_->broadcastToMembers(res, -1);
		return;
	}
	channel_->setPass("");
	channel_->setProtected(false);
	modeResponse(_client.getNickname(), cmd,channel_, "-k");
}

void Server:: modeCmd(std::vector<std::string> cmd, int clientSocket, Client &_client)
{
	std::string	res;
	char			modeType;
	std::string	types;

	if (cmd.size() < 2)
	{
		notEnoughParamsErr(clientSocket, _client.getNickname());
		return ;
	}
	Channel *channel_ = this->findChannel(cmd.at(1));
	if (channel_ == NULL)
	{
		res = ":ft_irc 403 " + _client.getNickname() + " " + cmd.at(1) + " :No such channel" + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
	else if (cmd.size() == 2)
	{
		res = ":ft_irc 324 " + _client.getNickname() + " MODE " + cmd.at(1) + " " + channel_->getModes() + "\r\n";
		psend(clientSocket, res.c_str(), res.size(), 0);
	}
	else if (!channel_->hasOperator(clientSocket))
	{
		std::string msg = ":ft_irc 482 " + _client.getNickname() + " " + cmd.at(1) + " :You're not channel operator" + "\r\n";
		psend(clientSocket, msg.c_str(), msg.size(), 0);
	}
	else
	{
		modeType = cmd.at(2).at(0);
		if (modeType != '+' && modeType != '-')
		{
			unkownCmd(clientSocket ,_client.getNickname(), cmd, modeType);
			return;
		}
		types = cmd.at(2).substr(1);
		int paramIndex = 3;
		for (int i = 0; types[i] && i < (int)types.size(); i++)
		{
			if (types[i] == 'i')
				inviteMode(modeType ,channel_, cmd, _client);
			else if (types[i] == 't')
				topicMode(modeType ,channel_, cmd, _client);
			else if (types[i] == 'o' && modeType == '+')
			{
				if (cmd.size() <= (size_t)paramIndex)
				{
					notEnoughParamsErr(clientSocket, _client.getNickname());
					continue ;
				}
				operatorMode(modeType, cmd[paramIndex] ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else if (types[i] == 'o' && modeType == '-')
			{
				if (cmd.size() <= (size_t)paramIndex)
				{
					notEnoughParamsErr(clientSocket, _client.getNickname());
					continue ;
				}
				operatorMode(modeType, cmd[paramIndex] ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else if (types[i] == 'l'  && modeType == '+')
			{
				if (cmd.size() <= (size_t)paramIndex)
				{
					notEnoughParamsErr(clientSocket, _client.getNickname());
					continue ;
				}
				limitMode(modeType, cmd[paramIndex] ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else if (types[i] == 'l' && modeType == '-')
			{
				limitMode(modeType, "null" ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else if (types[i] == 'k' && modeType == '+')
			{
				if (cmd.size() <= (size_t)paramIndex)
				{
					notEnoughParamsErr(clientSocket, _client.getNickname());
					continue ;
				}
				keyMode(modeType, cmd[paramIndex] ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else if (types[i] == 'k' && modeType == '-')
			{
				keyMode(modeType, "" ,channel_, clientSocket, cmd, _client);
				paramIndex++;
			}
			else
				unkownCmd(clientSocket ,_client.getNickname(), cmd, types[i]);
		}
	}
}
