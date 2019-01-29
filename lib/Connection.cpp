#include "Connection.h"
#include "messageQueue.h"
#include "serviceBase.h"



Connection::Connection()
{
	SetConnStatus(false);
}

Connection::~Connection()
{

}

int32 Connection::GetConnectionID()
{
	return m_ConnID;
}

int64 Connection::GetConnectionData()
{

}

int32 Connection::GetConnStatus()
{
	return m_ConnComplete;
}

void Connection::SetConnStatus(bool status)
{
	m_ConnComplete = status;

	m_fd = -1;

	bzero(m_RecvBuf,RECV_BUF_SIZE);

	m_RecvOffIndex = 0;

	m_nRecvSize = 0;

}

void Connection::SetConnectionID(int32 dwConnID)
{
	m_ConnID = dwConnID;
}

void Connection::SetConnectionData(int64 dwData)
{

}

bool Connection::Close()
{

}

bool Connection::SetSocket(int32 hSocket)
{
	m_fd = hSocket;
	m_ConnComplete = true;
}

int32 Connection::GetSocket()
{
	return m_fd;
}

bool Connection::ExtractBuffer()
{

}

bool Connection::DoReceive()
{
	long long length = 0;
	char buffer[BUFFER_SIZE];
	bzero(buffer,BUFFER_SIZE);

	uint32 tempLen = 0;
	while ((length = recv(m_fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		tempLen += length;
	
		if ((m_nRecvSize + length) > RECV_BUF_SIZE)
		{
			if ((m_nRecvSize - m_RecvOffIndex) > 0)
			{
				memmove(m_RecvBuf, m_RecvBuf + m_RecvOffIndex, m_nRecvSize - m_RecvOffIndex);
				m_nRecvSize = m_nRecvSize - m_RecvOffIndex;
				m_RecvOffIndex = 0;
			}
			else
			{
				bzero(m_RecvBuf, RECV_BUF_SIZE);
				m_RecvOffIndex = 0;
				m_nRecvSize = 0;
			}
		}

		memcpy(m_RecvBuf + m_nRecvSize, buffer, length);
		m_nRecvSize += length;

		while ((m_nRecvSize - m_RecvOffIndex) >= sizeof(NetPacketHeader))
		{

			NetPacketHeader* pHeader = (NetPacketHeader*)(m_RecvBuf + m_RecvOffIndex);
			if (pHeader == nullptr)
			{
				continue;
			}

			if (pHeader->wCode != NET_CODE)
			{
				getchar();
				break;
			}
			if (pHeader->wDataSize > (m_nRecvSize - m_RecvOffIndex))
			{

				getchar();
				break;
			}

			uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

			char* pData = MemoryManager::GetInstancePtr()->GetFreeMemoryArr(datalen);

			memcpy(pData, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);

			ServiceBase::GetInstancePtr()->AddNetPackToQueue(m_ConnID, datalen, (uint32)pHeader->wOpcode, pData);

			m_RecvOffIndex += pHeader->wDataSize;
		}
	}

	if (tempLen <= 0)
	{
		return false;
	}
	return true;
}

void Connection::EventCallBack(const int& m_efd,func fun)
{
	if (m_events->events & EPOLLIN)
	{
		std::cout << "call back " << std::endl;
		if (!DoReceive())
		{
			std::cout << "call back " << std::endl;
			fun();
		}
		else
		{
			/*
			std::cout << "EPOLLOUT " << std::endl;
			m_events->events = EPOLLOUT | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
			*/
		}
		
		return;
	}

	if (m_events->events & EPOLLOUT)
	{
		std::cout << "DoSend " << std::endl;

		switch (DoSend())
		{
		case SendComplete:
			m_events->events = EPOLLIN | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
			break;

		case SendPart:
			m_events->events = EPOLLOUT | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
			break;

		case SendError:
			fun();
			break;
		}

		return;
	}
}

bool Connection::IsConnectionOK()
{

}

bool Connection::SetConnectionOK(bool bOk)
{

}

bool Connection::Clear()
{

}

bool Connection::SendBuffer(NetPacket* pBuff)
{
	AUTOMUTEX
	memcpy(m_SendBuf, (char*)pBuff, pBuff->Header.wDataSize);
	m_nSendSize += pBuff->Header.wDataSize;

	std::cout << "pBuff->Header.wDataSize : " <<pBuff->Header.wDataSize << std::endl;
	std::cout << "m_nSendSize : " << m_nSendSize << std::endl;
	return true;
}

SendStatus Connection::DoSend()
{
	AUTOMUTEX
	if(m_SendOffIndex < m_nSendSize)
	{
		int wlen = send(m_fd, m_SendBuf, m_nSendSize, 0);
		if (wlen == 0)
		{
			return SendError;
		}

		m_SendOffIndex += wlen;
	}
	if (m_SendOffIndex == m_nSendSize)
	{
		m_nSendSize = 0;
		m_SendOffIndex = 0;
		return SendComplete;
	}
	return SendPart;
}

int32 Connection::GetFd()
{
	return m_fd;
}

void Connection::SetEpollEv(struct epoll_event* ev)
{
	m_events = ev;
}

struct epoll_event* Connection::GetEpollEv()
{
	return m_events;
}
