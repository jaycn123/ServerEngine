#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"


class CGameService : public IPacketDispatcher
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	void		Init();

	void		Uninit();

	void		Run();

	bool		DispatchPacket(CNetPacket* pNetPacket);

public:

	
public:

	//*********************消息处理定义开始******************************
	bool OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket);
	//*********************消息处理定义结束******************************

	void testSendProtobuf(uint32 connid);

};

#endif
