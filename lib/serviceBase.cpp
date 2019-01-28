#include "serviceBase.h"

ServiceBase::ServiceBase(void)
{
	m_pPacketDispatcher = nullptr;
}

ServiceBase::~ServiceBase(void)
{

}

ServiceBase* ServiceBase::GetInstancePtr()
{
	static ServiceBase serviceBase;
	return &serviceBase;
}

bool ServiceBase::StartNetWork(uint32 port, uint32 maxConnNum, IPacketDispatcher* pDispather)
{
	if (pDispather == nullptr)
	{
		return false;
	}
	m_pPacketDispatcher = pDispather;

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
	
}

bool ServiceBase::Run()
{
	StartThreadParsing();
	m_ConnManager.Run();
}

bool ServiceBase::StopNetWork()
{
	printf(" LOOP EXIT!!! \n");
	m_ConnManager.Close();
	return true;
}

bool ServiceBase::AddNetPackToQueue(uint32 connid, uint32 len, uint32 messid, char* pdata)
{
	if (m_PackNum < MAXPACKNUM)
	{
		AUTOMUTEX
		m_NetPackArr[m_PackNum++] = std::move(CNetPacket(connid, len, messid, pdata));
	}
	else
	{
		AUTOMUTEX
		m_NetPackQueue.push(std::move(CNetPacket(connid, len, messid, pdata)));
	}
}

void ServiceBase::StartThreadParsing()
{
	std::thread tPars(&ServiceBase::ParsingLoop,this);
	tPars.detach();
}

void ServiceBase::ParsingLoop()
{
	while (true)
	{
		{
			AUTOMUTEX
			ParsingNetPack();
		}
		usleep(1000);
	}
}

void ServiceBase::ParsingNetPack()
{
	if (m_PackNum != 0)
	{
		std::cout << "m_PackNum : " << m_PackNum << std::endl;
	}
	while (m_ReadIndex < m_PackNum)
	{
		m_pPacketDispatcher->DispatchPacket(&(m_NetPackArr[m_ReadIndex++]));
	}
	m_PackNum = 0;
	m_ReadIndex = 0;
}

