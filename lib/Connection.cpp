#include "Connection.h"
#include "messageQueue.h"
#include "serviceBase.h"
#include "../protoFiles/test.pb.h"




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
	m_lastRecvTime = NowTime;
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
	m_lastRecvTime = NowTime;
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
				std::cout << "发送完成 ----------" << std::endl;
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

				std::cout << "m_nRecvSize-------------" << m_nRecvSize << std::endl;

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


				std::cout <<"m_nRecvSize : " << m_nRecvSize << " m_RecvOffIndex : " << m_RecvOffIndex << std::endl;
				std::cout << "清理数据 ----------" << std::endl;
			}
			else
			{
				bzero(m_RecvBuf, RECV_BUF_SIZE);
				m_RecvOffIndex = 0;
				m_nRecvSize = 0;
				std::cout << "发送完成 ----------" << std::endl;
			}
		}

		length = recv(m_fd, m_RecvBuf + m_nRecvSize, RECV_BUF_SIZE - m_nRecvSize, 0);
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
					std::cout << "m_nRecvSize : " << m_nRecvSize << std::endl;
					std::cout << "m_RecvOffIndex : " << m_RecvOffIndex << std::endl;

					getchar();
					break;
				}

				if (pHeader->wDataSize > (m_nRecvSize - m_RecvOffIndex))
				{
					break;
				}

				//std::cout << "m_nRecvSize-------------" << m_nRecvSize << " length : " << length << std::endl;

				uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

				char* pData = MemoryManager::GetInstancePtr()->GetFreeMemoryArr(datalen);
				//std::cout << "pData addr " << static_cast<const void *>(pData) << endl;
				//char recvbuff[1024] = { 0 };
				//memcpy(recvbuff, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);
				memcpy(pData, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);

				/*
				HeartBeatReq Req;
				Req.ParsePartialFromArray(recvbuff, datalen);

				std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;
				*/
				ServiceBase::GetInstancePtr()->AddNetPackToQueue(m_ConnID, datalen, (uint32)pHeader->wOpcode, pData);

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
		if (!DoReceiveEx())
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
			getchar();
			std::cout << "zxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzxzx" << std::endl;
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
	NetPacket* pBuff = nullptr;

	if (m_nSendSize == 0)
	{
		AUTOMUTEX
		while (!m_SendPackQueue.empty())
		{
			pBuff = m_SendPackQueue.front();
			memcpy(m_SendBuf + m_nSendSize, (char*)pBuff, pBuff->Header.wDataSize);
			m_nSendSize += pBuff->Header.wDataSize;
			m_SendPackQueue.pop();
			break;
		}
	}

	if(m_SendOffIndex < m_nSendSize)
	{
		int	wlen = send(m_fd, m_SendBuf + m_SendOffIndex, m_nSendSize - m_SendOffIndex, 0);
		
		if (wlen <= 0)
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
	else
	{
		if (m_SendOffIndex > m_nSendSize)
		{
			return SendError;
		}
	}
	return SendPart;
	
}

int32 Connection::GetFd()
{
	return m_fd;
}
