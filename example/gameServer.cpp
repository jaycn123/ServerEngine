#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "gameServer.h"
#include "../lib/Log.h"
#include "../lib/configFile.h"
#include "../protoFiles/msgId.pb.h"

GameServer::GameServer(void)
{
	
}

GameServer::~GameServer(void)
{

}

GameServer* GameServer::GetInstancePtr()
{
	static GameServer cGameService;
	return &cGameService;
}

void GameServer::Init()
{

	if (!CLog::GetInstancePtr()->StartLog("gameServer", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	CLog::GetInstancePtr()->LogError("---------server start-----------");

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("�����ļ�����ʧ��!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("game_log_level"));

	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_port");
	uint32_t maxcon = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_max_con");
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("gate_svr_ip");

	std::cout << "port : " << port << " maxcon : " << maxcon << std::endl;

	ServiceBase::GetInstancePtr()->StartNetWork(ip, port, maxcon,this);

	RegisterMsg();
}

void GameServer::Uninit()
{

}

void GameServer::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool GameServer::DispatchPacket(CNetPacket* pNetPacket)
{
	return true;
	switch (pNetPacket->messId)
	{
		PROCESS_MESSAGE_ITEMEX(1, OnMsgWatchHeartBeatReq)
	}
}

void GameServer::OnSecondTimer()
{

}

void GameServer::OnCloseConnect(Connection* pConn)
{

}

void GameServer::OnNewConnect(Connection* pConnection)
{

}

void GameServer::RegisterMsg()
{
	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_FIRST_REQ, std::bind(&GameServer::OnMsgWatchHeartBeatReq, this, std::placeholders::_1));
}

void GameServer::OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;


	//testSendProtobuf(pNetPacket->m_connId, Req.connid());
}

void GameServer::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	GameServer::GetInstancePtr()->Init();
	GameServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
