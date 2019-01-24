#include "Connection.h"

Connection::Connection()
{
	m_fd = -1;
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

void Connection::SetConnStatus(bool status)
{
	m_ConnComplete = status;
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

	while ((length = recv(m_fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if ((m_nRecvSize + length) > RECV_BUF_SIZE)
		{
			if ((m_nRecvSize - m_offindex) > 0)
			{
				memmove(m_RecvBuf, m_RecvBuf + m_offindex, m_nRecvSize - m_offindex);
				m_nRecvSize = m_nRecvSize - m_offindex;
				m_offindex = 0;
			}
			else
			{
				bzero(m_RecvBuf, RECV_BUF_SIZE);
				m_offindex = 0;
				m_nRecvSize = 0;
			}
		}

		memcpy(m_RecvBuf + m_nRecvSize, buffer, length);
		m_nRecvSize += length;

		while ((m_nRecvSize - m_offindex) >= sizeof(NetPacketHeader))
		{
			NetPacketHeader* pHeader = (NetPacketHeader*)(m_RecvBuf + m_offindex);
			if (pHeader == nullptr)
			{
				std::cout << " pHeader == nullptr " << std::endl;
				getchar();
				continue;
			}

			if (pHeader->wCode != NET_CODE)
			{
				std::cout << "m_nRecvSize : " << m_nRecvSize << std::endl;
				std::cout << "m_offindex : " << m_offindex << std::endl;
				std::cout << "m_tempCount : " << m_tempCount << std::endl;
				std::cout << " pHeader->wCode != NET_CODE " << std::endl;
				getchar();
				break;
			}
			if (pHeader->wDataSize > (m_nRecvSize - m_offindex))
			{
				break;
			}
			m_tempCount++;
			NetPacket* msg = (NetPacket*)(m_RecvBuf + m_offindex);
			m_offindex += pHeader->wDataSize;
		}
	}
}

void Connection::EventCallBack(const int& m_efd, struct epoll_event* m_events)
{
	if (m_events->events & EPOLLIN)
	{
		std::cout << "DoReceive " << std::endl;
		DoReceive();

		m_events->events = EPOLLOUT | EPOLLET;
		epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
		sleep(1);
		return;
	}

	if (m_events->events & EPOLLOUT)
	{
		std::cout << "DoSend " << std::endl;
		
		DoSend();

		m_events->events = EPOLLIN | EPOLLET;

		epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, m_events);
		sleep(1);
		
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
	send(m_fd, "aaa", 3, 0);

}

int32 Connection::GetFd()
{
	return m_fd;
}
