#include "Connection.h"
#include "serviceBase.h"
#include "../protoFiles/test.pb.h"
#include "memoryPool.h"




Connection::Connection()
{
	m_ConnComplete = false;
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

void Connection::SetConnectionID(int32 dwConnID)
{
	m_ConnID = dwConnID;
	m_lastRecvTime = NowTime;
}

void Connection::SetConnectionData(int64 dwData)
{

}

bool Connection::Close()
{
	ServiceBase::GetInstancePtr()->OnCloseConnect(this);

	AUTOMUTEX

	m_ConnComplete = false;

	m_fd = -1;

	//bzero(m_RecvBuf,RECV_BUF_SIZE);

	m_RecvOffIndex = 0;

	m_nRecvSize = 0;

	pSendMemoryAddr = nullptr;

	m_SendOffIndex = 0;

	m_nSendSize = 0;


	while (!m_SendPackQueue.empty())
	{
		MemoryManager::GetInstancePtr()->FreeMemory(m_SendPackQueue.front()->Header.wDataSize, (char*)(m_SendPackQueue.front()));
		m_SendPackQueue.pop();
	}

	MemoryManager::GetInstancePtr()->FreeMemory(RECV_BUF_SIZE, m_pRecvBuf);
}

bool Connection::SetSocket(int32 hSocket)
{
	m_fd = hSocket;
	m_ConnComplete = true;
	m_pRecvBuf = MemoryManager::GetInstancePtr()->GetFreeMemory(RECV_BUF_SIZE);
	ServiceBase::GetInstancePtr()->OnNewConnect(this);
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
	int32 length = 0;
	while (true)
	{
		if (RECV_BUF_SIZE == m_nRecvSize)
		{
			if ((m_nRecvSize - m_RecvOffIndex) > 0)
			{
				memmove(m_pRecvBuf, m_pRecvBuf + m_RecvOffIndex, m_nRecvSize - m_RecvOffIndex);
				m_nRecvSize = m_nRecvSize - m_RecvOffIndex;
				m_RecvOffIndex = 0;


				//std::cout << "m_nRecvSize : " << m_nRecvSize << " m_RecvOffIndex : " << m_RecvOffIndex << std::endl;
				//std::cout << "清理数据 ----------" << std::endl;
			}
			else
			{
				bzero(m_pRecvBuf, RECV_BUF_SIZE);
				m_RecvOffIndex = 0;
				m_nRecvSize = 0;
			}
		}

		length = recv(m_fd, m_pRecvBuf + m_nRecvSize, RECV_BUF_SIZE - m_nRecvSize, 0);
		if (length == 0)
		{
			std::cout << "recv leng : " << length << std::endl;
			//client close conn
			return false;
		}

		if (length > 0)
		{
			m_nRecvSize += length;
			while ((m_nRecvSize - m_RecvOffIndex) >= sizeof(NetPacketHeader))
			{
				NetPacketHeader* pHeader = (NetPacketHeader*)(m_pRecvBuf + m_RecvOffIndex);
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
					std::cout << "m_nRecvSize : " << m_nRecvSize << std::endl;
					std::cout << "m_RecvOffIndex : " << m_RecvOffIndex << std::endl;

					getchar();
					break;
				}

				if (pHeader->wDataSize > (m_nRecvSize - m_RecvOffIndex))
				{
					break;
				}

				uint32_t datalen = pHeader->wDataSize - sizeof(NetPacketHeader);
				CNetPacket* pData = (CNetPacket*)MemoryManager::GetInstancePtr()->GetFreeMemory(datalen + sizeof(CNetPacket));
				pData->m_connId = m_ConnID;
				pData->m_len = datalen;
				pData->messId = pHeader->wOpcode;
				memcpy(pData->m_pData, m_pRecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);
				ServiceBase::GetInstancePtr()->AddNetPackToQueue(pData);
				m_RecvOffIndex += pHeader->wDataSize;
			}
		}
		else
		{
			if (length == -1 && errno != EAGAIN)
			{
				perror("read error");
				getchar();
				return false;
			}
			else
			{
				m_lastRecvTime = NowTime;

				return true;
			}
		}
	}
	m_lastRecvTime = NowTime;

	return true;
}

void Connection::EventCallBack(const int& m_efd, struct epoll_event* pEv, func fun)
{
	if (pEv->events & EPOLLIN)
	{
		if (!DoReceive())
		{
			fun();
		}
	
		return;
	}

	if (pEv->events & EPOLLOUT)
	{
		switch (DoSend())
		{
		case SendComplete:
		{
			struct epoll_event en;
			en.data.ptr = this;
			en.events = EPOLLIN | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, GetFd(), &en);
			break;
		}

		case SendPart:
		{
			struct epoll_event en;
			en.data.ptr = this;
			en.events = EPOLLOUT | EPOLLET;
			epoll_ctl(m_efd, EPOLL_CTL_MOD, GetFd(), &en);
			break;
		}
		case SendError:
		{
			fun();
			break;
		}
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
	{
		AUTOMUTEX
		m_SendPackQueue.push(pBuff);
	}
	return true;
}



SendStatus Connection::DoSend()
{
	AUTOMUTEX
	std::cout << "DoSendEx : pack : "<< m_SendPackQueue.size() << std::endl;
	while (!m_SendPackQueue.empty())
	{
		NetPacket* pSendMemoryAddr = m_SendPackQueue.front();
		int	wlen = send(m_fd, (char*)pSendMemoryAddr + m_SendOffIndex, pSendMemoryAddr->Header.wDataSize - m_SendOffIndex, 0);
		if (wlen == 0)
		{
			std::cout << "555555555555555555555" << std::endl;
			return SendError;
		}

		if (wlen < 0)
		{
			if (errno == EAGAIN)
			{
				//perror("read error");
				//getchar();
				return SendPart;
			}
			else
			{
			//	perror("read error");  //read error: Broken pipe
				return SendError;
			}
		}


		m_SendOffIndex += wlen;

		if (m_SendOffIndex == pSendMemoryAddr->Header.wDataSize)
		{
			m_SendOffIndex = 0;
			MemoryManager::GetInstancePtr()->FreeMemory(pSendMemoryAddr->Header.wDataSize, (char*)pSendMemoryAddr);
			m_SendPackQueue.pop();
		}
	}
	return SendComplete;
}

int32 Connection::GetFd()
{
	return m_fd;
}
