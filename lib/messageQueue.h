#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include "type_define.h"



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
		m_pData = pdata;
	}

	uint32     m_connId;
	uint32     m_len;
	uint32     messId;
	char*      m_pData;

};

struct IPacketDispatcher
{
	virtual bool DispatchPacket(CNetPacket* pNetPacket) = 0;
	/*
	virtual bool OnSecondTimer() = 0;
	virtual bool OnCloseConnect(CConnection* pConnection) = 0;
	virtual bool OnNewConnect(CConnection* pConnection) = 0;
	*/
};

#endif