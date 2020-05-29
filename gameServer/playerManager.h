#ifndef  PLAYERMANAGER_H
#define  PLAYERMANAGER_H

#include "player.h"
#include "../lib/serviceBase.h"
#include "../protoFiles/msgId.pb.h"
#include "../protoFiles/player.pb.h"


class PlayerManager 
{
private:

	PlayerManager(void);

	virtual ~PlayerManager(void);

public:

	void Init(MysqlControl* pMysql);

	static PlayerManager* GetInstancePtr();

	void RegisterMsg();

	void PlayerOnline(Player* pPlayer);

	void PlayerOffOnline(Player* pPlayer);

	Player* GetPlayerByConnId(int32_t connid);

	Player* GetPlayerByRoleID(int32_t roleid);

public:

	void OnCreatePlayer(CNetPacket* pNetPacket);

	void OnPlayerOnline(CNetPacket* pNetPacket);

	void OnPlayerOffline(CNetPacket* pNetPacket);

	void InitPlayer();

private:

	std::map<int32_t, int32_t> m_ConnectMap;

	MysqlControl* m_pMysql = nullptr;

	std::map < uint32_t, Player* > m_PlayerMap;

};

#endif