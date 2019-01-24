#ifndef _CONNECTION_H_
#define _CONNECTION_H_
#include "./Platform.h"
#include "netpack.h"

#define  NET_MSG_RECV				1
#define  NET_MSG_SEND				2
#define  NET_MSG_CONNECT			3

#define RECV_BUF_SIZE               10240

#define BUFFER_SIZE                 1024

#define Hash_Map                    std::map


class Connection
{
public:
	Connection();

	virtual ~Connection();

public:

	int32   GetConnectionID();

	int64   GetConnectionData();

	int32   GetConnStatus();

	void    SetConnStatus(bool status);

	void    SetConnectionID(int32 dwConnID);

	void	SetConnectionData(int64 dwData);

	bool	Close();

	bool	SetSocket(int32 hSocket);

	int32   GetSocket();

	bool	ExtractBuffer();

	bool	DoReceive();

	void    EventCallBack(uint32_t enents);

	void	EventCallBack(const int& m_efd,struct epoll_event* m_events);

	bool	IsConnectionOK();

	bool	SetConnectionOK(bool bOk);

	bool    Clear();

	bool    SendBuffer(NetPacketHeader*	pBuff);

	bool    SendMessage(int32 dwMsgID, int64 uTargetID, int32 dwUserData, const char* pData, int32 dwLen);

	bool    DoSend(NetPacketHeader* pBuff);

	bool    DoSend();

	int32   GetFd();

public:

	int32	m_fd;

	int32	m_ConnID;

	bool	m_ConnComplete;

	char    m_RecvBuf[RECV_BUF_SIZE];

	uint32  m_offindex = 0;

	uint32  m_nRecvSize = 0;

	uint32  m_tempCount = 0;
};

#endif
