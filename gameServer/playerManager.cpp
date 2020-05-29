#include "playerManager.h"

class PlayerManager;

PlayerManager::PlayerManager(void)
{

}

PlayerManager::~PlayerManager(void)
{

}


void PlayerManager::Init(MysqlControl* pMysql)
{
	m_pMysql = pMysql;
}

PlayerManager* PlayerManager::GetInstancePtr()
{
	static PlayerManager m_playerManager;
	return &m_playerManager;
}


void PlayerManager::RegisterMsg()
{
	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_CREATEPLAYER_REQ, std::bind(&PlayerManager::OnCreatePlayer, this, std::placeholders::_1));

	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_LOGINPLAYER_ACK, std::bind(&PlayerManager::OnPlayerOnline, this, std::placeholders::_1));

	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_LOGINOFF_REQ, std::bind(&PlayerManager::OnPlayerOffline, this, std::placeholders::_1));
}

void PlayerManager::PlayerOnline(Player* pPlayer)
{
	m_PlayerMap[pPlayer->GetRoleID()] = pPlayer;
}

void PlayerManager::PlayerOffOnline(Player* pPlayer)
{
	m_PlayerMap.erase(pPlayer->GetRoleID());
}

Player* PlayerManager::GetPlayerByConnId(int32_t connid)
{
	auto it = m_ConnectMap.find(connid);
}

Player* PlayerManager::GetPlayerByRoleID(int32_t roleid)
{

}

void PlayerManager::OnCreatePlayer(CNetPacket* pNetPacket)
{

}

void PlayerManager::OnPlayerOnline(CNetPacket* pNetPacket)
{
	/*
	Msg_LoginPlayer_Ack Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);
	std::cout << Req.roleid() << std::endl;
	Msg_Login_Ack Ack;
	Ack.set_returncode(6789);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pNetPacket->m_connId,pNetPacket->m_targetid, MessageID::MSGID_LOGINPLAYER_ACK, Ack);
	
	m_ConnectMap[pNetPacket->m_connId] = Req.roleid();
	*/
}

void PlayerManager::OnPlayerOffline(CNetPacket* pNetPacket)
{

}

void PlayerManager::InitPlayer()
{
	


}

