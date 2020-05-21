#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"

enum ConnType
{
	CT_Client = 1,
	CT_GameServer = 2
};

struct GameInfo
{
	uint32_t m_serverId = 0;
	uint32_t m_connId = 0;
	std::string m_serverName = "";
};


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

	void        RegisterMsg();

	void        InitMsg();

protected:


	
public:

	bool OnForwardNetPack(CNetPacket* pNetPacket);

	//*********************消息处理定义开始******************************
	void OnHeartBeatReq(CNetPacket* pNetPacket);

	void OnServerInfo(CNetPacket* pNetPacket);

	//*********************消息处理定义结束******************************

private:

	uint32_t m_AccountConnID = 0;

	std::map<int32, Connection* >m_ClientConnectionMap;

	std::map<uint32_t, GameInfo*> m_GameServerIdMap;

	std::map<uint32_t, GameInfo*> m_GameConnIdMap;

};

#endif
