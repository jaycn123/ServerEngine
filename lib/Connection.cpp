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

	m_RecvoffIndex = 0;

	m_nRecvSize = 0;

	m_tempCount = 0;

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
		if (length <= 0)
		{
			std::cout << "length : " << length << std::endl;
		}
		tempLen += length;
	
		if ((m_nRecvSize + length) > RECV_BUF_SIZE)
		{
			if ((m_nRecvSize - m_RecvoffIndex) > 0)
			{
				memmove(m_RecvBuf, m_RecvBuf + m_RecvoffIndex, m_nRecvSize - m_RecvoffIndex);
				m_nRecvSize = m_nRecvSize - m_RecvoffIndex;
				m_RecvoffIndex = 0;
			}
			else
			{
				bzero(m_RecvBuf, RECV_BUF_SIZE);
				m_RecvoffIndex = 0;
				m_nRecvSize = 0;
			}
		}

		memcpy(m_RecvBuf + m_nRecvSize, buffer, length);
		m_nRecvSize += length;

		while ((m_nRecvSize - m_RecvoffIndex) >= sizeof(NetPacketHeader))
		{
			NetPacketHeader* pHeader = (NetPacketHeader*)(m_RecvBuf + m_RecvoffIndex);
			if (pHeader == nullptr)
			{
				std::cout << " pHeader == nullptr " << std::endl;
				getchar();
				continue;
			}

			if (pHeader->wCode != NET_CODE)
			{
				std::cout << " pHeader->wCode != NET_CODE " << std::endl;
				getchar();
				break;
			}
			if (pHeader->wDataSize > (m_nRecvSize - m_RecvoffIndex))
			{
				break;
			}

			uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

			char* pData = MemoryManager::GetInstancePtr()->GetFreeMemoryArr(datalen);

			memcpy(pData, m_RecvBuf + m_RecvoffIndex + sizeof(NetPacketHeader), datalen);

			ServiceBase::GetInstancePtr()->AddNetPackToQueue(m_ConnID, datalen, (uint32)pHeader->wOpcode, pData);

			m_RecvoffIndex += pHeader->wDataSize;
		}
	}

	if (tempLen <= 0)
	{
		return false;
	}
	return true;
}

void Connection::EventCallBack(const int& m_efd, struct epoll_event* m_events, func fun)
{
	if (m_events->events & EPOLLIN)
	{
		std::cout << "DoReceive " << std::endl;
		if (!DoReceive())
		{
			fun();
		}
		else
		{
			m_events->events = EPOLLOUT | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
		}
		
		return;
	}

	if (m_events->events & EPOLLOUT)
	{
		std::cout << "DoSend " << std::endl;
		DoSend();
		m_events->events = EPOLLIN | EPOLLET;
		epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
		
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

bool Connection::SendBuffer(NetPacketHeader* pBuff)
{

}

bool Connection::SendMessage(int32 dwMsgID, int64 uTargetID, int32 dwUserData, const char* pData, int32 dwLen)
{

}

bool Connection::DoSend()
{
	int len = send(m_fd, "aaa", 3, 0);
	std::cout << "send len : " << len << std::endl;
}

int32 Connection::GetFd()
{
	return m_fd;
}
