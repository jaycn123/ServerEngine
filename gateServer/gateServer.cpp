#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "gateServer.h"
#include "../lib/Log.h"
#include "../lib/CommonFunc.h"
#include "../protoFiles/msgId.pb.h"
#include <functional>


GateServer::GateServer(void)
{
	
}

GateServer::~GateServer(void)
{

}

GateServer* GateServer::GetInstancePtr()
{
	static GateServer cGameService;
	return &cGameService;
}

void GateServer::Init()
{
	if (!CLog::GetInstancePtr()->StartLog("gateServer", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	CLog::GetInstancePtr()->LogError((char*)("---------server start-----------"));

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("配制文件加载失败!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("gate_log_level"));

	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("gate_svr_port");
	uint32_t maxcon = CConfigFile::GetInstancePtr()->GetIntValue("gate_svr_max_con");
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("gate_svr_ip");

	std::cout << "port : " << port << " maxcon : "<< maxcon << std::endl;

	ServiceBase::GetInstancePtr()->StartNetWork(ip , port, maxcon, this);
}

void GateServer::Uninit()
{

}

void GateServer::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool GateServer::DispatchPacket(CNetPacket* pNetPacket)
{
	OnForwardNetPack(pNetPacket);
}

void GateServer::OnSecondTimer()
{

}

void GateServer::OnCloseConnect(Connection* pConnection)
{
	if (m_ClientConnectionMap.find(pConnection->GetConnectionID()) != m_ClientConnectionMap.end())
	{
		m_ClientConnectionMap.erase(pConnection->GetConnectionID());
		return;
	}

	if (m_GameConnIdMap.find(pConnection->GetConnectionID()) != m_GameConnIdMap.end())
	{
		std::cout << "gameserver close connection Id " << m_GameConnIdMap[pConnection->GetConnectionID()]->m_serverId << std::endl;
		return;
	}

	if (pConnection->GetConnectionID() == m_AccountConnID)
	{
		std::cout << "account close connection Id " << std::endl;
		return;
	}
}


void GateServer::OnNewConnect(Connection* pConnection)
{
	if (m_GameConnIdMap.find(pConnection->GetConnectionID()) == m_GameConnIdMap.end())
	{
		if (m_AccountConnID != pConnection->GetConnectionID())
		{
			m_ClientConnectionMap[pConnection->GetConnectionID()] = pConnection;
		}
	}
}

void GateServer::RegisterMsg()
{
	 ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_HEARTBEAT_REQ, std::bind(&GateServer::OnHeartBeatReq, this, std::placeholders::_1));
	 ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_CONNETC_INFO, std::bind(&GateServer::OnServerInfo, this, std::placeholders::_1));
}

void GateServer::InitMsg()
{
	GateServer::GetInstancePtr()->RegisterMsg();
}

bool GateServer::OnForwardNetPack(CNetPacket* pNetPacket)
{
	std::cout << "pNetPacket : " << pNetPacket->messId << std::endl;
	if (pNetPacket->messId > MessageID::MSGID_LOGINMSG_BEGIN&& pNetPacket->messId < MessageID::MSGID_LOGINMSG_END)
	{
		std::cout << m_AccountConnID << std::endl;
		std::cout << pNetPacket->m_connId << std::endl;

		if (m_AccountConnID != 0 && pNetPacket->m_connId != m_AccountConnID)
		{
			std::cout << "1111111111111" << std::endl;
			ServiceBase::GetInstancePtr()->SendDataByConnID(m_AccountConnID, pNetPacket);
			return true;
		}
		else
		{
			std::cout << "2222222222" << pNetPacket->m_targetid << std::endl;
			ServiceBase::GetInstancePtr()->SendDataByConnID(pNetPacket->m_targetid, pNetPacket);
			return true;
		}
	}

	return true;
}

void GateServer::OnHeartBeatReq(CNetPacket* pNetPacket)
{
	Msg_Heartbeat_Req Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	Msg_Heartbeat_Ack ack;
	ack.set_index(Req.index());
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pNetPacket->m_connId, MessageID::MSGID_HEARTBEAT_ACK, ack);
}

void GateServer::OnServerInfo(CNetPacket* pNetPacket)
{
	std::cout << "OnServerInfo" << std::endl;
	Msg_Connetc_Info req;
	req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	switch (req.stype())
	{
	case ST_Client:
		break;
	case ST_Account:
	{
		m_AccountConnID = pNetPacket->m_connId;
	}
		break;
	case ST_Game:
	{
		GameInfo* pServer = new GameInfo();
		pServer->m_connId = pNetPacket->m_connId;
		pServer->m_serverId = req.serverid();
		pServer->m_serverName = req.name();
		m_GameServerIdMap[req.serverid()] = pServer;
		m_GameConnIdMap[pServer->m_connId] = pServer;
	}
		break;

	default:
		break;
	}
}

int main()
{
	GateServer::GetInstancePtr()->Init();
	GateServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
