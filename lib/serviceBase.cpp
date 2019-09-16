#include "serviceBase.h"
#include "../protoFiles/test.pb.h"

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
	signal(SIGPIPE, SIG_IGN);
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

bool ServiceBase::AddNetPackToQueue(CNetPacket* pData)
{
	{
		AUTOMUTEX
		m_NetPackQueue.push(pData);
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
	while (!m_NetPackQueue.empty())
	{
		CNetPacket* pData = m_NetPackQueue.front();
		m_pPacketDispatcher->DispatchPacket(pData);
		MemoryManager::GetInstancePtr()->FreeMemory(pData->m_len, (char*)pData);
		m_NetPackQueue.pop();
	}
}

