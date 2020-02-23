#ifndef ACCOUNTSERVER_H
#define ACCOUNTSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"
#include "../lib/mysql_control.h"
#include "accountManager.h"


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

	bool        ConnectionGame();

	bool        ConnectionGate();

	void        OnCloseConnect(Connection* pConnection);

	void		OnNewConnect(Connection* pConnection);

	void        InitMsg();
	
public:

	bool OnForwardNetPack(CNetPacket* pNetPacket);

	//*********************消息处理定义开始******************************
	bool OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket);
	//*********************消息处理定义结束******************************

	void testSendProtobuf(uint32 connid, uint32_t tempcount);

private:

	MysqlControl* m_pMysql = nullptr;

	uint32_t      m_GateServerConnID = 0;

	uint32_t      m_GameServerConnID = 0;

};

#endif
