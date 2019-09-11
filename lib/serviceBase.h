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
	
	bool   AddNetPackToQueue(CNetPacket* pData);

	bool   SendMsgProtoBuf(uint32 dwConnID, uint32 dwMsgID,const google::protobuf::Message& pdata);

protected:

	void   StartThreadParsing();

	void   ParsingLoop();

	void   ParsingNetPack();


protected:


	IPacketDispatcher *            m_pPacketDispatcher;

	std::queue<CNetPacket*>        m_NetPackQueue;

	std::mutex                     m_mutex;

	uint32                         m_checkConnStatus = 0;
};


#endif /*__SERVICE_BASE_H__*/