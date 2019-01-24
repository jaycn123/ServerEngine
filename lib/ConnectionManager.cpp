#include "ConnectionManager.h"

ConnectionManager::ConnectionManager()
{
	m_listener = -1;
	m_epfd = -1;
	m_threadPool.setMaxQueueSize(100000);
	m_threadPool.start(3);
}

ConnectionManager::~ConnectionManager()
{

}

bool ConnectionManager::CreteSocket(xstring& ip, int32 port)
{
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//创建监听socket
	m_listener = socket(PF_INET, SOCK_STREAM, 0);
	if (m_listener < 0)
	{
		printf("error m_listener : %d\n", m_listener);
		return false;
	}
	else
	{
		if (setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &serverAddr, sizeof(serverAddr)) < 0)
		{
			perror("setsockopet error\n");
			return false;
		}
		//绑定地址
		if (bind(m_listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			printf("error bind m_listener : %d\n", m_listener);
			return false;
		}
		//监听
		int ret = listen(m_listener, 5);
		if (ret < 0)
		{
			printf("error listen m_listener : %d\n", m_listener);
			return false;
		}
		printf("%s Start to listen\n", ip.c_str());
		return true;
	}
}

bool ConnectionManager::CreateEpollEvent()
{
	//在内核中创建事件表
	m_epfd = epoll_create(EPOLL_SIZE);
	if (m_epfd < 0)
	{
		printf("epoll created, epollfd = %d\n", m_epfd);
		return false;
	}

	printf("epoll created, epollfd = %d\n", m_epfd);
	//static struct epoll_event events[EPOLL_SIZE];
	//往内核事件表里添加事件
	AddEpollFd(true);
	return true;
}

void ConnectionManager::AddEpollFd(bool enable_et)
{
	struct epoll_event ev;
	ev.data.fd = m_listener;
	ev.events = EPOLLIN;     //输入触发epoll-event
	if (enable_et)
	{
		ev.events = EPOLLIN | EPOLLET;
	}
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listener, &ev);
	setnonblocking(m_listener);
}

void ConnectionManager::AddEpollFd(int fd, bool enable_et)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	if (enable_et)
	{
		ev.events = EPOLLIN | EPOLLOUT |EPOLLET;
	}
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev);
	setnonblocking(fd);
}

void ConnectionManager::sendBroadcastmessage(int32 clientfd)
{
	char buf[BUF_SIZE], message[BUF_SIZE];
	//清零
	bzero(buf, BUF_SIZE);
	bzero(message, BUF_SIZE);

	printf("read from client(clientID = %d)\n", clientfd);
	int len = recv(clientfd, buf, BUF_SIZE, 0);

	std::cout << "len : " << len << std::endl;
	if (len == 0)  // len = 0 client关闭了连接
	{
		close(clientfd);
		//list删除fd
	}
	else  //进行广播
	{
		int len = send(clientfd, "haha", BUF_SIZE, 0);
	}
}

int ConnectionManager::setnonblocking(int sockfd)
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
	return 0;
}

void ConnectionManager::Init()
{
	m_Looptask = std::bind(&ConnectionManager::Run, this);
}

void ConnectionManager::Start()
{
	m_threadPool.run(m_Looptask);
}

void ConnectionManager::Run()
{
	printf("to Run !!!!!!\n");
	while (1)
	{
		//epoll_events_count表示就绪事件的数目
		int epoll_events_count = epoll_wait(m_epfd, m_events, EPOLL_SIZE, -1);
		if (epoll_events_count < 0)
		{
			perror("epoll failure");
			break;
		}

		//printf("epoll_events_count = %d\n", epoll_events_count);
		//处理这epoll_events_count个就绪事件
		for (int i = 0; i < epoll_events_count; ++i)
		{
			int sockfd = m_events[i].data.fd;

			//新用户连接
			if (m_events[i].data.fd == m_listener)
			{
				struct sockaddr_in client_address;
				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = accept(m_listener, (struct sockaddr*)&client_address, &client_addrLength);

				if(clientfd < 0)
				{
					printf("cleintfd: %d\n", clientfd);
					continue;
				}

				printf("client connection from: %s : % d(IP : port), clientfd = %d \n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), clientfd);
				printf("Add new clientfd = %d to epoll\n", clientfd);

				AddNewConn(clientfd);
			}
			else
			{
				if (m_events[i].events&EPOLLIN)
				{
					m_ConnectionVec[m_ConnectionMap[sockfd] - 1]->EventCallBack(m_epfd, &(m_events[i]));
					m_events[i].data.fd = sockfd;
					//设置用于注测的写操作事件

					m_events[i].events = EPOLLOUT | EPOLLET;
					//修改sockfd上要处理的事件为EPOLLOUT

					epoll_ctl(m_epfd,EPOLL_CTL_MOD,sockfd, &(m_events[i]));
				}

				if (m_events[i].events&EPOLLOUT)
				{
					m_ConnectionVec[m_ConnectionMap[sockfd] - 1]->EventCallBack(m_epfd, &(m_events[i]));
					m_events[i].data.fd = sockfd;
					//设置用于注测的读操作事件

					m_events[i].events = EPOLLIN | EPOLLET;
					//修改sockfd上要处理的事件为EPOLIN

					epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockfd, &(m_events[i]));
				}
			}
		}
	}
}

void ConnectionManager::SetConnectionNum(int32 nMaxCons)
{
	m_ConnectionVec.assign(nMaxCons, NULL);
	for (int32 i = 0; i < nMaxCons; ++i)
	{
		Connection* pConn = new Connection();
		pConn->SetConnectionID(i + 1);
		m_ConnectionVec[i] = pConn;
	}
}

void ConnectionManager::Close()
{
	close(m_listener); //关闭socket
	close(m_epfd);    //关闭内核
}

bool ConnectionManager::CanExit()
{
	return m_threadPool.canExit();
}

bool ConnectionManager::SendConnIDToClient(int32 fd, int32 connID)
{
	/*
	NetPacket_Test1 msg;
	msg.nIndex = connID;
	strncpy(msg.arrMessage, "test", sizeof(msg.arrMessage));
	char* pDataBuffer = (char*)&msg;
	NetPacketHeader* pHead = (NetPacketHeader*)m_cbSendBuf;
	pHead->wOpcode = NET_TEST1;

	memcpy(pHead + 1, pDataBuffer, sizeof(pDataBuffer));g
	// 发送消息
	const unsigned short nSendSize = sizeof(pDataBuffer) + sizeof(NetPacketHeader);
	pHead->wDataSize = nSendSize;
	int ret = send(fd, m_cbSendBuf, nSendSize, 0);
	return (ret > 0) ? true : false;
	*/
}

void ConnectionManager::AddNewConn(int32 fd)
{
	AddEpollFd(fd, true);
	for (int32 i = 0; i < m_ConnectionVec.size(); ++i)
	{
		Connection* pConn = m_ConnectionVec.at(i);
		if (!pConn->GetConnStatus())
		{
			pConn->SetSocket(fd);
			std::cout << "connid : " <<pConn->GetConnectionID() << std::endl;
			m_ConnectionMap[fd] = pConn->GetConnectionID();
			//SendConnIDToClient(fd, pConn->GetConnectionID());
			break;
		}
	}
}

Connection* ConnectionManager::GetConnByFd(int32 fd)
{
	for (int32 i = 0; i < m_ConnectionVec.size(); ++i)
	{
		Connection* pConn = m_ConnectionVec.at(i);
		if (pConn->GetFd() == fd)
		{
			return pConn;
		}
	}
	return nullptr;
}

Connection* ConnectionManager::GetConnByConnid(int32 nConnid)
{
	if (nConnid >= m_ConnectionVec.size())
	{
		return nullptr;
	}
	return m_ConnectionVec[nConnid];
}
