#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "chatServer.h"
#include "../lib/Log.h"
#include "../lib/configFile.h"

CGameService::CGameService(void)
{
	
}

CGameService::~CGameService(void)
{

}

CGameService* CGameService::GetInstancePtr()
{
	static CGameService cGameService;
	return &cGameService;
}

void CGameService::Init()
{

	if (!CLog::GetInstancePtr()->StartLog("gameServer", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	CLog::GetInstancePtr()->LogError("---------server start-----------");

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("配制文件加载失败!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("game_log_level"));

	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_port");
	uint32_t maxcon = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_max_con");


	std::cout << "port : " << port << " maxcon : " << maxcon << std::endl;
	ServiceBase::GetInstancePtr()->StartNetWork(8888,10000,this);
}

void CGameService::Uninit()
{

}

void CGameService::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool CGameService::DispatchPacket(CNetPacket* pNetPacket)
{
	switch (pNetPacket->messId)
	{
		PROCESS_MESSAGE_ITEMEX(1, OnMsgWatchHeartBeatReq)
	}
}

bool CGameService::OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	//std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;

	static uint32_t count = 1;

	//testSendProtobuf(pNetPacket->m_connId, Req.connid());
}

void CGameService::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	CGameService::GetInstancePtr()->Init();
	CGameService::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}