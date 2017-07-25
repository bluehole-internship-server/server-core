#pragma once
#include <vector>
#include <algorithm>
#include "Client.h"
#include "memory_pool.hpp"

class ClientManager
{
public:
	ClientManager();
	~ClientManager();
	Client * NewClient();
	VOID DeleteClient(Client *);
	VOID AddClient(Client *);
	VOID RemoveClient(Client *);

private:
	std::vector<Client *>clients_;
	core::ObjectPool<Client> client_pool_;
};

