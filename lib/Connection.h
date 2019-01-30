#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "type_define.h"
#include "netpack.h"
#include "memoryPool.h"

#include <functional>

#define  NET_MSG_RECV				1
#define  NET_MSG_SEND				2
#define  NET_MSG_CONNECT			3

#define RECV_BUF_SIZE               102400

#define SEND_BUF_SIZE               102400

#define BUFFER_SIZE                 1024

#define Hash_Map                    std::map

class Connection
{
public:
	Connection();

	virtual ~Connection();

public:

	int32       GetConnectionID();
			    
	int64       GetConnectionData();
			    
	int32       GetConnStatus();
			    
	void        SetConnStatus(bool status);
			    
	void        SetConnectionID(int32 dwConnID);
			    
	void	    SetConnectionData(int64 dwData);
			    
	bool	    Close();
			    
	bool	    SetSocket(int32 hSocket);
			    
	int32       GetSocket();
			    
	bool	    ExtractBuffer();
			    
	bool	    DoReceive();
			    
	void	    EventCallBack(const int& m_efd,func fun);
			    
	bool	    IsConnectionOK();
			    
	bool	    SetConnectionOK(bool bOk);
			    
	bool        Clear();
			    
	bool        SendBuffer(NetPacket*	pBuff);

	SendStatus  DoSend();

	int32       GetFd();

	void        SetEpollEv(struct epoll_event* ev);

	struct epoll_event* GetEpollEv();
	
public:

	struct epoll_event*     m_events;

	int32	                m_fd;
			                
	int32	                m_ConnID;
			                
	bool	                m_ConnComplete;
			                
	char                    m_RecvBuf[RECV_BUF_SIZE];
			                
	uint32                  m_RecvOffIndex = 0;
			                
	uint32                  m_nRecvSize = 0;
			                
	uint32                  m_tempCount = 0;

	//-----------------------------

	char                    m_SendBuf[SEND_BUF_SIZE];

	uint32                  m_SendOffIndex = 0;

	uint32                  m_nSendSize = 0;

	std::queue<NetPacket*>   m_SendPackQueue;

	std::mutex              m_mutex;

};

#endif
