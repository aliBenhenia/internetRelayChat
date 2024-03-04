#include <server.hpp>

void Server:: quiteCmd(int clientSocket, Client _client)
{
	std::vector<Channel*>::iterator	it = channels->begin();
	std::string								nickName = _client.getNickname();

	this->removeClient(clientSocket);
	while (it != channels->end())
	{
		if ((*it)->hasClient(clientSocket))
		{
			(*it)->removeClient(nickName);
			(*it)->removeOperator(nickName);
			if ((*it)->hasNoClient() == false) {
				delete *it;
				channels->erase(it);
				continue ;
			}
		}
		it++;
	}
}