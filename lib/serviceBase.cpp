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

	ConnectionManager::GetInstancePtr()->SetConnectionNum(maxConnNum);
	xstring ip = "0.0.0.0";
	if (!ConnectionManager::GetInstancePtr()->CreteSocket(ip, port))
	{
		printf("create socket error \n");
		return false;
	}

	if (!ConnectionManager::GetInstancePtr()->CreateEpollEvent())
	{
		printf("create CreateEpollEvent error \n");
		return false;
	}
	
}

bool ServiceBase::Run()
{
	StartThreadParsing();
	ConnectionManager::GetInstancePtr()->Run();
}

bool ServiceBase::StopNetWork()
{
	printf(" LOOP EXIT!!! \n");
	ConnectionManager::GetInstancePtr()->Close();
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

	return true;
}

bool ServiceBase::SendMsgProtoBuf(uint32 dwConnID, uint32 dwMsgID, const google::protobuf::Message& pdata)
{
	char szBuff[102400] = { 0 };

	if(pdata.ByteSize() > 102400)
	{
		return false;
	}

	pdata.SerializePartialToArray(szBuff, pdata.GetCachedSize());

	ConnectionManager::GetInstancePtr()->sendMessageByConnID(dwConnID, dwMsgID, szBuff, pdata.GetCachedSize());
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
		m_checkConnStatus++;
		if (m_checkConnStatus >= 5)
		{
			m_checkConnStatus = 0;
		//	ConnectionManager::GetInstancePtr()->CheckConntionAvalible();  //  有bug  处理数据太大 连接超时
		}
		{
			AUTOMUTEX
			ParsingNetPack();
		}
		
		usleep(2500);
	}
}

void ServiceBase::ParsingNetPack()
{
	if (m_PackNum > 0)
	{
		std::cout << "m_PackNum : " << m_PackNum << std::endl;
	}
	while (m_ReadIndex < m_PackNum)
	{
		m_pPacketDispatcher->DispatchPacket(&(m_NetPackArr[m_ReadIndex]));
		MemoryManager::GetInstancePtr()->FreeMemoryArr(m_NetPackArr[m_ReadIndex].m_len, m_NetPackArr[m_ReadIndex].m_pData);
		m_ReadIndex++;
	}
	m_PackNum = 0;
	m_ReadIndex = 0;
}

