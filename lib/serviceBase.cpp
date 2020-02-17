#include "serviceBase.h"
#include "../protoFiles/test.pb.h"
#include "CommonFunc.h"

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

bool ServiceBase::StartNetWork(std::string& ip, uint32 port, uint32 maxConnNum, IPacketDispatcher* pDispather)
{
	signal(SIGPIPE, SIG_IGN);
	if (pDispather == nullptr)
	{
		return false;
	}
	m_pPacketDispatcher = pDispather;

	ConnectionManager::GetInstancePtr()->SetConnectionNum(maxConnNum);
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

bool ServiceBase::SendDataByConnID(uint32 connid, uint32 msgid, const char* pData, uint32 dwLen)
{
	return ConnectionManager::GetInstancePtr()->sendMessageByConnID(connid, msgid, pData, dwLen);
}

void ServiceBase::OnCloseConnect(Connection* pConnection)
{
	m_pPacketDispatcher->OnCloseConnect(pConnection);
}

void ServiceBase::OnNewConnect(Connection* pConnection)
{
	m_pPacketDispatcher->OnNewConnect(pConnection);
}

void ServiceBase::RegisterMsg(uint32_t msgid, msgfunc fp)
{
	m_msgFuncMap[msgid].push_back(fp);
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
		if (m_tickCount == 0)
		{
			m_tickCount = CommonFunc::GetTickCount();
		}
		else
		{
			if ((CommonFunc::GetTickCount() - m_tickCount) > 1000)
			{
				m_pPacketDispatcher->OnSecondTimer();
				m_tickCount = CommonFunc::GetTickCount();
			}
		}

		usleep(1000);
	}
}

void ServiceBase::ParsingNetPack()
{
	while (!m_NetPackQueue.empty())
	{
		CNetPacket* pData = m_NetPackQueue.front();
		ExecutionMsg(pData);
		m_pPacketDispatcher->DispatchPacket(pData);
		MemoryManager::GetInstancePtr()->FreeMemory(pData->m_len + sizeof(CNetPacket), (char*)pData);
		m_NetPackQueue.pop();
	}
}

bool ServiceBase::ExecutionMsg(CNetPacket* pNetPack)
{
	std::cout << "msgid : " << pNetPack->messId  << std::endl;
	auto it = m_msgFuncMap.find(pNetPack->messId);
	if (it == m_msgFuncMap.end())
	{
		std::cout << "msgid : " << pNetPack->messId << " not Register" << std::endl;
		return false;
	}

	for (auto& iter : it->second)
	{
		iter(pNetPack);
	}

	return true;
}

