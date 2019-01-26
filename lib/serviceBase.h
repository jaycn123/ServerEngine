#ifndef SERVICE_BASE_H
#define SERVICE_BASE_H

#define MAXPACKNUM 1000

#include "messageQueue.h"
#include "ConnectionManager.h"

class ServiceBase
{
protected:
	ServiceBase(void);

	virtual ~ServiceBase(void);

public:
	static ServiceBase* GetInstancePtr();

	bool   StartNetWork(uint32 port, uint32 maxConnNum);

	bool   StopNetWork();
	
	bool   AddNetPackToQueue(CNetPacket data);

protected:

	ConnectionManager  m_ConnManager;

	CNetPacket         m_NetPackArr[MAXPACKNUM];

	uint32             m_PackNum = 0;

	uint32             m_ReadIndex = 0;
};


#endif /*__SERVICE_BASE_H__*/