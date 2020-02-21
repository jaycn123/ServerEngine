#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "accountServer.h"
#include "../lib/Log.h"

AccountServer::AccountServer(void)
{
	
}

AccountServer::~AccountServer(void)
{

}

AccountServer* AccountServer::GetInstancePtr()
{
	static AccountServer cGameService;
	return &cGameService;
}

void AccountServer::Init()
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


	ServiceBase::GetInstancePtr()->StartNetWork(ip,port, maxcon, this);
}

void AccountServer::Uninit()
{

}

void AccountServer::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool AccountServer::DispatchPacket(CNetPacket* pNetPacket)
{
	OnForwardNetPack(pNetPacket);
// 	switch (pNetPacket->messId)
// 	{
// 		PROCESS_MESSAGE_ITEMEX(1, OnMsgWatchHeartBeatReq)
// 	}

}

void AccountServer::OnSecondTimer()
{

}

void AccountServer::OnCloseConnect(Connection* pConnection)
{

}

void AccountServer::OnNewConnect(Connection* pConnection)
{

}

void AccountServer::InitMsg()
{
	
}

bool AccountServer::OnForwardNetPack(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;

	testSendProtobuf(pNetPacket->m_connId, Req.connid());
	return true;
}

bool AccountServer::OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;

	testSendProtobuf(pNetPacket->m_connId, Req.connid());
}

void AccountServer::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	AccountServer::GetInstancePtr()->Init();
	AccountServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
