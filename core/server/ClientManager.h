#pragma once
#include <vector>
#include <algorithm>
#include "Client.h"

class ClientManager
{
public:
	ClientManager();
	~ClientManager();
	VOID AddClient(Client *);
	VOID DeleteClient(Client *);

private:
	std::vector<Client *>clients_;
};

