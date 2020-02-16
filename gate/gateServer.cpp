#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "gateServer.h"
#include "../lib/Log.h"
#include "../lib/CommonFunc.h"
#include "../protoFiles/msgId.pb.h"


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
// 	switch (pNetPacket->messId)
// 	{
// 		PROCESS_MESSAGE_ITEMEX(1, OnMsgWatchHeartBeatReq)
// 	}
}

void GateServer::OnSecondTimer()
{
	if (m_GameConnID == 0)
	{
		ConnectionGame();
	}
}

void GateServer::OnCloseConnect(Connection* pConnection)
{

}

void GateServer::OnNewConnect(Connection* pConnection)
{

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
		std::cout << "pNetPacket->messId : "<<pNetPacket->messId << std::endl;
		ServiceBase::GetInstancePtr()->SendDataByConnID(m_GameConnID, pNetPacket->messId, pNetPacket->m_pData, pNetPacket->m_len);
	}

	/*
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;


	testSendProtobuf(pNetPacket->m_connId, Req.connid());
	*/

	return true;
}

bool GateServer::OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;


	testSendProtobuf(pNetPacket->m_connId, Req.connid());
}

void GateServer::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	GateServer::GetInstancePtr()->Init();
	GateServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
