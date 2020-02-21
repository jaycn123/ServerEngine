#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "type_define.h"
#include "Connection.h"

struct CNetPacket
{
	uint32     m_connId = 0;
	uint32     m_len = 0;
	uint32     messId = 0;
	uint32     m_targetid = 0;
	char       m_pData[0];

};

struct IDataHandler
{
	virtual void OnCloseConnect(Connection* pConnection) = 0;
	virtual void OnNewConnect(Connection* pConnection) = 0;
};

struct IPacketDispatcher
{
	virtual void InitMsg() = 0;
	virtual bool DispatchPacket(CNetPacket* pNetPacket) = 0;
	virtual void OnSecondTimer() = 0;
	virtual void OnCloseConnect(Connection* pConnection) = 0;
	virtual void OnNewConnect(Connection* pConnection) = 0;
};

#endif