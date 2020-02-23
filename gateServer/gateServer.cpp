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
	if (pConnection->m_ConnID == m_GameConnID)
	{
		m_GameConnID = 0;
		std::cout << "gameserver close connection" << std::endl;
	}
	else
	{
		m_ClientConnectionMap.erase(pConnection->GetConnectionID());
		Msg_Connetc_Notice Req;
		Req.set_connid(pConnection->m_ConnID);
		Req.set_status(0);
		ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GameConnID, MessageID::MSGID_CONNETC_NOTICE, Req);
	}
}

void GateServer::OnNewConnect(Connection* pConnection)
{
	if (pConnection->GetConnectionID() == m_GameConnID)
	{
		pConnection->SetConnType(CT_GameServer);
	}
	else
	{
		pConnection->SetConnType(CT_Client);
		m_ClientConnectionMap[pConnection->GetConnectionID()] = pConnection;

		Msg_Connetc_Notice Req;
		Req.set_connid(pConnection->m_ConnID);
		Req.set_status(1);
		ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GameConnID, MessageID::MSGID_CONNETC_NOTICE, Req);
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

bool GateServer::ConnectionGame()
{
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("game_svr_ip");
	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_port");
	auto pConn = ConnectionManager::GetInstancePtr()->ConnectionToServer(ip, port);
	if (!pConn)
	{
		CLog::GetInstancePtr()->LogError("connect game fail ! ");
		return false;
	}

	m_GameConnID = pConn->m_ConnID;

	return true;
}

bool GateServer::OnForwardNetPack(CNetPacket* pNetPacket)
{
	if (pNetPacket->messId > MessageID::MSGID_GAMEMSG_BEGIN && pNetPacket->messId < MessageID::MSGID_GAMEMSG_END)
	{
		if (m_GameConnID != 0 && pNetPacket->m_connId != m_GameConnID)
		{
			std::cout << "pNetPacket->messId : " << pNetPacket->messId << std::endl;
			ServiceBase::GetInstancePtr()->SendDataByConnID(m_GameConnID, pNetPacket);
			return true;
		}
		else
		{
			auto it = m_ClientConnectionMap.find(pNetPacket->m_targetid);
			if (it == m_ClientConnectionMap.end())
			{
				return true;
			}

			std::cout << "pNetPacket->messId : " << pNetPacket->messId <<" pNetPacket->m_targetid "<< pNetPacket->m_targetid << std::endl;
			ServiceBase::GetInstancePtr()->SendDataByConnID(pNetPacket->m_targetid, pNetPacket);
		}
	}

	return true;
}

void GateServer::OnHeartBeatReq(CNetPacket* pNetPacket)
{
	std::cout << "OnHeartBeatReq : " << std::endl;
	Msg_Heartbeat_Ack ack;
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pNetPacket->m_connId, MessageID::MSGID_HEARTBEAT_ACK, ack);
}

void GateServer::OnServerInfo(CNetPacket* pNetPacket)
{
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
		m_GameConnIdMap[req.serverid()] = pNetPacket->m_connId;
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
