#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "type_define.h"
#include "Connection.h"

struct CNetPacket
{
	CNetPacket()
	{

	}

	CNetPacket(uint32 connid, uint32 len, uint32 messid)
	{
		m_connId = connid;
		m_len = len;
		messId = messid;
	}

	uint32     m_connId;
	uint32     m_len;
	uint32     messId;
	char       m_pData[0];

};

struct IDataHandler
{
	virtual void OnCloseConnect(Connection* pConnection) = 0;
	virtual void OnNewConnect(Connection* pConnection) = 0;
};

struct IPacketDispatcher
{
	virtual bool DispatchPacket(CNetPacket* pNetPacket) = 0;
	virtual void OnSecondTimer() = 0;
	virtual void OnCloseConnect(Connection* pConnection) = 0;
	virtual void OnNewConnect(Connection* pConnection) = 0;
};

#endif