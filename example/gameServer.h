#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/Connection.h"


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

public:

	
public:

	//*********************��Ϣ�����忪ʼ******************************
	bool OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket);
	//*********************��Ϣ���������******************************

	void testSendProtobuf(uint32 connid, uint32_t tempcount);

};

#endif
