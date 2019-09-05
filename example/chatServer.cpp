#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "chatServer.h"
#include "../lib/Log.h"

CGameService::CGameService(void)
{
	if (!CLog::GetInstancePtr()->StartLog("ChatServer", "log"))
	{
		std::cout << "error " << std::endl;
	}
	CLog::GetInstancePtr()->LogError("---------server start-----------");
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
	ServiceBase::GetInstancePtr()->StartNetWork(8888,1000,this);
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

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;
	//testSendProtobuf(pNetPacket->m_connId);
}

void CGameService::testSendProtobuf(uint32 connid)
{
	HeartBeatReq Req;
	Req.set_connid(666);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	CGameService::GetInstancePtr()->Init();
	CGameService::GetInstancePtr()->Run();
	return 0;
}