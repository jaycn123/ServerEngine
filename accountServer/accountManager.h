#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"


class AccountManager 
{
private:
	AccountManager(void);
	virtual ~AccountManager(void);

public:
	static AccountManager* GetInstancePtr();

	void		Init();

	void		Uninit();

	void        InitMsg();
	
public:

	//bool OnForwardNetPack(CNetPacket* pNetPacket);
};

#endif
