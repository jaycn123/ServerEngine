#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/Connection.h"
#include "playerManager.h"


class GameServer : public IPacketDispatcher
{
private:
	GameServer(void);
	virtual ~GameServer(void);

public:
	static GameServer* GetInstancePtr();

	void		Init();

	void		Uninit();

	void		Run();

	bool		DispatchPacket(CNetPacket* pNetPacket);

	void        OnSecondTimer();

	void        OnCloseConnect(Connection* pConnection);

	void		OnNewConnect(Connection* pConnection);


	void        InitMsg();

	void        RegisterMsg();


protected:

	bool ConnectionGate();

	
public:

	//*********************消息处理定义开始******************************
	void OnConnectNotice(CNetPacket* pNetPacket);
	//*********************消息处理定义结束******************************

	void testSendProtobuf(uint32 connid, uint32_t tempcount);

private:

	
	uint32_t m_GateServerConnId = 0;

};

#endif
