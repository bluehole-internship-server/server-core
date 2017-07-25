#include "ClientManager.h"



ClientManager::ClientManager()
{
}


ClientManager::~ClientManager()
{
}

VOID ClientManager::AddClient(Client * client)
{
	clients_.push_back(client);
	puts("Added.");
}

VOID ClientManager::DeleteClient(Client * client)
{
	auto it = std::find(clients_.begin(), clients_.end(), client);
	if (it != clients_.end()) {
		delete *it;
		clients_.erase(it);
		puts("Deleted.");
	}
}
