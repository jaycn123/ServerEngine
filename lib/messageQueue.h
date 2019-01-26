#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include "type_define.h"

struct IPacketDispatcher
{
	/*
	virtual bool DispatchPacket(NetPacket* pNetPacket) = 0;
	virtual bool OnSecondTimer() = 0;
	virtual bool OnCloseConnect(CConnection* pConnection) = 0;
	virtual bool OnNewConnect(CConnection* pConnection) = 0;
	*/
};


struct CNetPacket
{
	CNetPacket()
	{

	}

	CNetPacket(uint32 connid, uint32 len, uint32 messid, char* pdata)
	{
		m_connId = connid;
		m_len = len;
		messId = messid;
	}

	uint32     m_connId;
	uint32     m_len;
	uint32     messId;
	char*      m_pData;
};
#endif