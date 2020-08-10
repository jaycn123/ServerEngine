#include "serviceBase.h"
#include "../protoFiles/test.pb.h"
#include "CommonFunc.h"

ServiceBase::ServiceBase(void)
{
	m_pPacketDispatcher = nullptr;
	m_SqlDataVec.reserve(DTN_Max);
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
	m_pPacketDispatcher->InitMsg();

	return true;
	
}

bool ServiceBase::InitClientConn(uint32 maxConnNum, IPacketDispatcher* pDispather)
{
	signal(SIGPIPE, SIG_IGN);
	if (pDispather == nullptr)
	{
		return false;
	}

	m_pPacketDispatcher = pDispather;

	ConnectionManager::GetInstancePtr()->SetConnectionNum(maxConnNum);

	if (!ConnectionManager::GetInstancePtr()->CreateEpollEvent())
	{
		printf("create CreateEpollEvent error \n");
		return false;
	}

	return true;
}

bool ServiceBase::Run()
{
	StartThreadParsing();

	if (pipe(m_pipe) == -1)
	{
		printf("pipe error !");
		exit(-1);
	}

	pid_t fpid = fork();
	if (fpid < 0)
	{
		printf("fork error !");
		exit(-1);
	}

	if (fpid != 0)
	{
		close(m_pipe[0]);
		ConnectionManager::GetInstancePtr()->Run();
	}
	else
	{
		StartDBThread();

		close(m_pipe[1]);
		m_sqlCache = (char*)malloc(4096 * 16);
		WriteDataBase();
	}
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

	if(dwConnID == 0 || pdata.ByteSize() > 102400)
	{
		return false;
	}

	pdata.SerializePartialToArray(szBuff, pdata.GetCachedSize());

	ConnectionManager::GetInstancePtr()->sendMessageByConnID(dwConnID,0, dwMsgID, szBuff, pdata.GetCachedSize());
}

bool ServiceBase::SendMsgProtoBuf(uint32 dwConnID, uint32_t sconnid, uint32 dwMsgID, const google::protobuf::Message& pdata)
{
	char szBuff[102400] = { 0 };

	if (pdata.ByteSize() > 102400)
	{
		return false;
	}

	pdata.SerializePartialToArray(szBuff, pdata.GetCachedSize());

	ConnectionManager::GetInstancePtr()->sendMessageByConnID(dwConnID, sconnid, dwMsgID, szBuff, pdata.GetCachedSize());

}
bool ServiceBase::SendMsgProtoBuf(CNetPacket* pNetPacket, uint32 dwMsgID, const google::protobuf::Message& pdata)
{
	char szBuff[102400] = { 0 };

	if (pdata.ByteSize() > 102400)
	{
		return false;
	}

	pdata.SerializePartialToArray(szBuff, pdata.GetCachedSize());

	ConnectionManager::GetInstancePtr()->sendMessageByConnID(pNetPacket->m_connId, pNetPacket->m_targetid, dwMsgID, szBuff, pdata.GetCachedSize());

}

bool ServiceBase::SendDataByConnID(uint32 connid, CNetPacket* pNetPacket)
{
	if (connid == 0)
	{
		return false;
	}
	return ConnectionManager::GetInstancePtr()->sendMessageByConnID(connid, pNetPacket->m_connId, pNetPacket->messId, pNetPacket->m_pData, pNetPacket->m_len);
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

void ServiceBase::ChangeDB(std::string& sql)
{
	m_DataBaseVec.push_back(sql);
}

void ServiceBase::ChangeDB(SqlData* sqldata)
{
	int len = write(m_pipe[1], (char*)sqldata, sqldata->m_len);
	if (len < sqldata->m_len)
	{
		std::cout << "ChangeDB error" << std::endl;
	}
}

void ServiceBase::SetMysqlControl(MysqlControl* pMysql)
{
	m_pMysqlControl = pMysql;
}

void ServiceBase::WriteDataBase()
{
	int32_t recvindex = 0;
	int32_t readindex = 0;

	while (true)
	{
		int len = read(m_pipe[0], m_sqlCache + recvindex, 4096);
		recvindex += len;

		while ((recvindex - readindex) > 0)
		{
			SqlData* data = (SqlData*)m_sqlCache + readindex;
			readindex += data->m_len;
			auto sql = std::string(data->m_sql);
			std::cout << data->m_len << " sql : " << sql << std::endl;

			AUTOMUTEX
			if(data->m_optype == CT_Delete)
			{
				if (m_SqlDataVec[data->m_datanum][data->m_gmid].find(CT_Add) != m_SqlDataVec[data->m_datanum][data->m_gmid].end())
				{
					m_SqlDataVec[data->m_datanum][data->m_gmid].clear();
				}
				else
				{
					m_SqlDataVec[data->m_datanum][data->m_gmid][data->m_optype] = data->m_sql;
					m_SqlDataVec[data->m_datanum][data->m_gmid].erase(CT_Modify);
				}
			}
			else
			{
				m_SqlDataVec[data->m_datanum][data->m_gmid][data->m_optype] = data->m_sql;
				std::cout << data->m_datanum << " " << data->m_gmid << " " << data->m_optype << std::endl;
			}
		}
	}
}

void ServiceBase::StartThreadParsing()
{
	std::thread tPars(&ServiceBase::ParsingLoop,this); 
	tPars.detach();
}

void ServiceBase::StartDBThread()
{
	std::thread tPars(&ServiceBase::ChangeDateBase, this);
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

void ServiceBase::ChangeDateBase()
{
	while (true)
	{
		{
			AUTOMUTEX
			for (uint32_t i = 0; i < m_SqlDataVec.size(); i++)
			{
				for (auto iter = m_SqlDataVec[i].begin(); iter != m_SqlDataVec[i].end(); iter++)
				{
					for (auto it = iter->second.begin(); it != iter->second.end(); it++)
					{
						m_pMysqlControl->Query(it->second);
					}
				}
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
	auto it = m_msgFuncMap.find(pNetPack->messId);
	if (it == m_msgFuncMap.end())
	{
		return false;
	}

	for (auto& iter : it->second)
	{
		iter(pNetPack);
	}

	return true;
}

