#include "serviceBase.h"

ServiceBase::ServiceBase(void)
{

}

ServiceBase::~ServiceBase(void)
{

}

ServiceBase* ServiceBase::GetInstancePtr()
{
	static ServiceBase serviceBase;
	return &serviceBase;
}

bool ServiceBase::StartNetWork(uint32 port, uint32 maxConnNum)
{
	m_ConnManager.SetConnectionNum(maxConnNum);
	xstring ip = "0.0.0.0";
	if (!m_ConnManager.CreteSocket(ip, port))
	{
		printf("create socket error \n");
		return false;
	}

	if (!m_ConnManager.CreateEpollEvent())
	{
		printf("create CreateEpollEvent error \n");
		return false;
	}

	m_ConnManager.Run();
}

bool ServiceBase::StopNetWork()
{
	printf(" LOOP EXIT!!! \n");
	m_ConnManager.Close();
	return true;
}

bool ServiceBase::AddNetPackToQueue(CNetPacket data)
{
	m_NetPackArr[m_PackNum] = data;
	m_PackNum++;
}
