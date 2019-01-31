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

	//uint32 tempLen = 0;

	std::cout << "DoReceive  " << std::endl;
	while ((length = recv(m_fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		std::cout << "len : " << length << std::endl;
		if (errno == EAGAIN)
		{
			//std::cout << "EAGAIN" << std::endl;
			return true;
		}
		if (length == 0)
		{
			return false;
		}

		//tempLen += length;
	
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
				std::cout << "pHeader == nullptr" << std::endl;
				getchar();
				continue;
			}

			if (pHeader->wCode != NET_CODE)
			{
				std::cout << "pHeader->wCode" << std::endl;
				getchar();
				break;
			}
			if (pHeader->wDataSize > (m_nRecvSize - m_RecvOffIndex))
			{
				break;
			}

			uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

			char* pData = MemoryManager::GetInstancePtr()->GetFreeMemoryArr(datalen);

			memcpy(pData, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);

			ServiceBase::GetInstancePtr()->AddNetPackToQueue(m_ConnID, datalen, (uint32)pHeader->wOpcode, pData);

			m_RecvOffIndex += pHeader->wDataSize;
		}
	}
	return true;
}

bool Connection::DoReceiveEx()
{
	int32 length = 0;
	while (true)
	{
		if (RECV_BUF_SIZE == m_nRecvSize)
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

		length = recv(m_fd, m_RecvBuf + m_nRecvSize, RECV_BUF_SIZE - m_nRecvSize, 0);
		if (length > 0)
		{
			m_nRecvSize += length;

			while ((m_nRecvSize - m_RecvOffIndex) >= sizeof(NetPacketHeader))
			{
				NetPacketHeader* pHeader = (NetPacketHeader*)(m_RecvBuf + m_RecvOffIndex);
				if (pHeader == nullptr)
				{
					std::cout << "pHeader == nullptr" << std::endl;
					getchar();
					continue;
				}

				if (pHeader->wCode != NET_CODE)
				{
					std::cout << "pHeader->wCode" << std::endl;

					std::cout << "length : " << length << std::endl;
					std::cout << "m_nRecvSize : "<< m_nRecvSize<<std::endl;
					std::cout << "m_RecvOffIndex : " << m_RecvOffIndex <<std::endl;

					getchar();
					break;
				}
				if (pHeader->wDataSize > (m_nRecvSize - m_RecvOffIndex))
				{
					break;
				}

				uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

				char* pData = MemoryManager::GetInstancePtr()->GetFreeMemoryArr(datalen);

				memcpy(pData, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);

				ServiceBase::GetInstancePtr()->AddNetPackToQueue(m_ConnID, datalen, (uint32)pHeader->wOpcode, pData);

				m_RecvOffIndex += pHeader->wDataSize;
			}
		}
		else
		{
			if (length == 0)
			{
				return false;
			}
			if (errno == EAGAIN)
			{
				return true;
			}
		}
	}
	return true;
}

void Connection::EventCallBack(const int& m_efd,func fun)
{
	if (m_events->events & EPOLLIN)
	{

		if (!DoReceiveEx())
		{
			fun();
		}

		return;
	}

	if (m_events->events & EPOLLOUT)
	{
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
	std::cout << "xxxxxxxxxxxxxxxxxxxxxx" << std::endl;
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
	{
		AUTOMUTEX
		m_SendPackQueue.push(pBuff);
	}
	return true;
}

SendStatus Connection::DoSend()
{
	NetPacket* pBuff = nullptr;
	AUTOMUTEX
	while (!m_SendPackQueue.empty())
	{
		pBuff = m_SendPackQueue.front();
		memcpy(m_SendBuf + m_nSendSize, (char*)pBuff, pBuff->Header.wDataSize);
		m_nSendSize += pBuff->Header.wDataSize;
		m_SendPackQueue.pop();
	}

	if(m_SendOffIndex < m_nSendSize)
	{
		int	wlen = send(m_fd, m_SendBuf, m_nSendSize, 0);
		
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
