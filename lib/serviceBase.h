#ifndef SERVICE_BASE_H
#define SERVICE_BASE_H

#include "messageQueue.h"
#include "ConnectionManager.h"
#include "type_define.h"

#define MAXPACKNUM 9999

#define PROCESS_MESSAGE_ITEMEX(dwMsgID, Func) \
		case dwMsgID:{\
		if(Func(pNetPacket)){return true;}}break;



class ServiceBase
{

protected:

	ServiceBase(void);

	virtual ~ServiceBase(void);

public:

	static ServiceBase* GetInstancePtr();

	bool   StartNetWork(uint32 port, uint32 maxConnNum, IPacketDispatcher* pDispather);

	bool   Run();

	bool   StopNetWork();
	
	bool   AddNetPackToQueue(uint32 connid, uint32 len, uint32 messid, char* pdata);

	bool   SendMsgProtoBuf(uint32 dwConnID, uint32 dwMsgID,const google::protobuf::Message& pdata);

protected:

	void   StartThreadParsing();

	void   ParsingLoop();

	void   ParsingNetPack();


protected:


	IPacketDispatcher *            m_pPacketDispatcher;

	CNetPacket                     m_NetPackArr[MAXPACKNUM];

	std::queue<CNetPacket>         m_NetPackQueue;

	uint32                         m_PackNum = 0;

	uint32                         m_ReadIndex = 0;

	std::mutex                     m_mutex;

	uint32                         m_checkConnStatus = 0;
};


#endif /*__SERVICE_BASE_H__*/