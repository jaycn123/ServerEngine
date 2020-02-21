#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"


class AccountServer : public IPacketDispatcher
{
private:
	AccountServer(void);
	virtual ~AccountServer(void);

public:
	static AccountServer* GetInstancePtr();

	void		Init();

	void		Uninit();

	void		Run();

	bool		DispatchPacket(CNetPacket* pNetPacket);

	void        OnSecondTimer();

	void        OnCloseConnect(Connection* pConnection);

	void		OnNewConnect(Connection* pConnection);

	void        InitMsg();
	
public:

	bool OnForwardNetPack(CNetPacket* pNetPacket);

	//*********************��Ϣ�����忪ʼ******************************
	bool OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket);
	//*********************��Ϣ���������******************************

	void testSendProtobuf(uint32 connid, uint32_t tempcount);

};

#endif
