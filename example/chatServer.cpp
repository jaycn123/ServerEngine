#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "chatServer.h"

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
	ServiceBase::GetInstancePtr()->StartNetWork(9997,5000,this);
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
	std::cout << "OnMsgWatchHeartBeatReq" << std::endl;
}

int main()
{
	CGameService::GetInstancePtr()->Init();
	CGameService::GetInstancePtr()->Run();
	return 0;
}