#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/Connection.h"


class Client : public IPacketDispatcher
{
private:
	Client(void);
	virtual ~Client(void);

public:
	static Client* GetInstancePtr();

	void		Init();

	void		Uninit();

	void		Run();

	bool		DispatchPacket(CNetPacket* pNetPacket);

	void        OnSecondTimer();

	void        OnCloseConnect(Connection* pConnection);

	void		OnNewConnect(Connection* pConnection);

	void        RegisterMsg();

	void        InitMsg();

public:



public:

	//*********************消息处理定义开始******************************
	void OnHeartBeatAck(CNetPacket* pNetPacket);

	void OnLoginAck(CNetPacket* pNetPacket);

	//*********************消息处理定义结束******************************

public:

	//*************************** Tets ***********************

	void Login();

protected:

	bool ConnectionGame();

	void HeartBeat();



private:

	int32   m_GameConnID = 0;

};

#endif
