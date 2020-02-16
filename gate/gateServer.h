#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"

class GateServer : public IPacketDispatcher
{
private:
	GateServer(void);
	virtual ~GateServer(void);

public:
	static GateServer* GetInstancePtr();

	void		Init();

	void		Uninit();

	void		Run();

	bool		DispatchPacket(CNetPacket* pNetPacket);

	void        OnSecondTimer();

	void        OnCloseConnect(Connection* pConnection);

	void		OnNewConnect(Connection* pConnection);

protected:

	bool        ConnectionGame();

	
public:

	bool OnForwardNetPack(CNetPacket* pNetPacket);

	//*********************��Ϣ�����忪ʼ******************************
	bool OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket);
	//*********************��Ϣ���������******************************

	void testSendProtobuf(uint32 connid, uint32_t tempcount);

private:

	uint32_t m_GameConnID = 0;

};

#endif
