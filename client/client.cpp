#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "client.h"
#include "../lib/Log.h"
#include "../lib/configFile.h"
#include "../protoFiles/msgId.pb.h"
#include "../protoFiles/player.pb.h"

Client::Client(void)
{
	
}

Client::~Client(void)
{

}

Client* Client::GetInstancePtr()
{
	static Client client;
	return &client;
}

void Client::Init()
{

	if (!CLog::GetInstancePtr()->StartLog("client", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("配制文件加载失败!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("game_log_level"));

	ServiceBase::GetInstancePtr()->InitClientConn(1,this);

}

void Client::Uninit()
{

}

void Client::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool Client::DispatchPacket(CNetPacket* pNetPacket)
{
	return true;
// 	switch (pNetPacket->messId)
// 	{
// 		PROCESS_MESSAGE_ITEMEX(1, OnMsgWatchHeartBeatReq)
// 	}
}

void Client::OnSecondTimer()
{
	if (m_GameConnID == 0)
	{
		ConnectionGame();
		return;
	}

	HeartBeat();
	Login();
}

void Client::OnCloseConnect(Connection* pConn)
{

}

void Client::OnNewConnect(Connection* pConnection)
{

}

void Client::RegisterMsg()
{
	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_HEARTBEAT_ACK, std::bind(&Client::OnHeartBeatAck, this, std::placeholders::_1));
	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_LOGIN_ACK, std::bind(&Client::OnLoginAck, this, std::placeholders::_1));
}

void Client::InitMsg()
{
	Client::GetInstancePtr()->RegisterMsg();
}

bool Client::ConnectionGame()
{
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("gate_svr_ip");
	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("gate_svr_port");
	auto pConn = ConnectionManager::GetInstancePtr()->ConnectionToServer(ip, port);
	if (!pConn)
	{
		CLog::GetInstancePtr()->LogError("connect game fail ! ");
		return false;
	}

	m_GameConnID = pConn->m_ConnID;

	return true;
}

void Client::OnHeartBeatAck(CNetPacket* pNetPacket)
{
	std::cout << "OnHeartBeatAck : " << std::endl;
}

void Client::OnLoginAck(CNetPacket* pNetPacket)
{
	Msg_Login_Ack Ack;
	Ack.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);
	std::cout << Ack.returncode() << std::endl;
}

void Client::HeartBeat()
{
	Msg_Heartbeat_Req Req;
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GameConnID, MessageID::MSGID_HEARTBEAT_REQ, Req);
}

void Client::Login()
{
	for (uint32_t i = 0; i < 1; i++)
	{
		Msg_Login_Req Req;
		Req.set_roleid(123456);
		ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GameConnID, MessageID::MSGID_LOGIN_REQ, Req);
	}
}

int main()
{
	Client::GetInstancePtr()->Init();
	Client::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
