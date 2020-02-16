#include "ConnectionManager.h"
#include "memoryPool.h"
#include "../protoFiles/test.pb.h"

ConnectionManager::ConnectionManager(void)
{
	m_listener = -1;
	m_epfd = -1;
	MemoryManager::GetInstancePtr()->Init();
}

ConnectionManager::~ConnectionManager(void)
{

}

ConnectionManager* ConnectionManager::GetInstancePtr()
{
	static ConnectionManager connectionManager;
	return &connectionManager;
}

bool ConnectionManager::CreteSocket(xstring& ip, int32 port)
{
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(port);
	//serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
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

void ConnectionManager::AddEpollFd(int fd,Connection* pConn, bool enable_et)
{
	struct epoll_event ev;
	ev.data.ptr = pConn;
	if (enable_et)
	{
		ev.events = EPOLLIN|EPOLLET;
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

// void ConnectionManager::Start()
// {
// 	std::cout << "-------------------" << std::endl;
// 	m_threadPool.run(m_Looptask);
// }

void ConnectionManager::Run()
{
	while (true)
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

			if (m_events[i].data.fd == m_listener)
			{
				struct sockaddr_in client_address;
				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = -1;
				while ((clientfd = accept(m_listener, (struct sockaddr*)&client_address, &client_addrLength)) > 0)
				{
					if (clientfd < 0)
					{
						printf("cleintfd: %d\n", clientfd);
						continue;
					}

					//printf("client connection from: %s : % d(IP : port), clientfd = %d \n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), clientfd);
					//printf("Add new clientfd = %d to epoll\n", clientfd);

					AddNewConn(clientfd);
				
					if (clientfd == -1)
					{
						if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
							perror("accept");
					}
				}
			}
			else
			{
				Connection* connTemp = (Connection*)(m_events[i].data.ptr);
				//std::cout << "connid : " << connTemp->GetConnectionID() << std::endl;
				auto fun = [&]() { epoll_ctl(m_epfd, EPOLL_CTL_DEL, connTemp->GetSocket(), &m_events[i]); FreeConnByConnid(connTemp->GetConnectionID()); close(connTemp->GetFd()); };
				connTemp->EventCallBack(m_epfd, &(m_events[i]),fun);
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
	std::cout << "ConnectionManager::Close()" << std::endl;
	close(m_listener); //关闭socket
	close(m_epfd);    //关闭内核
}


bool ConnectionManager::sendMessageByConnID(uint32 connid, uint32 msgid, const char* pData, uint32 dwLen)
{
	Connection* pConn = GetConnByConnid(connid);
	if (pConn == nullptr)
	{
		return false;
	}

	NetPacket* pMemData = (NetPacket*)MemoryManager::GetInstancePtr()->GetFreeMemory(dwLen + sizeof(NetPacketHeader));
	pMemData->Header.wOpcode = SENDDATA;
	pMemData->Header.wCode = NET_CODE;
	pMemData->Header.wDataSize = dwLen + sizeof(NetPacketHeader);
	memcpy(pMemData->pData, pData, dwLen);
	pConn->SendBuffer(pMemData);


	struct epoll_event en;
	en.data.ptr = pConn;
	en.events = EPOLLOUT | EPOLLET;
	epoll_ctl(m_epfd, EPOLL_CTL_MOD, pConn->GetSocket(), &en);	
}

void ConnectionManager::CheckConntionAvalible()
{
	uint64_t curTime = NowTime;
	for (int32 i = 0; i < m_ConnectionVec.size(); ++i)
	{
		Connection* pConn = m_ConnectionVec.at(i);
		if (pConn->GetConnStatus())
		{
			if (curTime > (pConn->m_lastRecvTime + 10))
			{
				std::cout << "CheckConntionAvalible" << std::endl;

				epoll_ctl(m_epfd, EPOLL_CTL_DEL, pConn->GetSocket(), &m_events[pConn->GetConnectionID() -1]);

				FreeConnByConnid(pConn->GetConnectionID()); 

				close(pConn->GetFd());
			}
		}
	}
}

Connection* ConnectionManager::ConnectionToServer(std::string& ip, uint32 port)
{
	int32 sockfd;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "create socket fail" << endl;
		return nullptr;
	}

	//setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	servaddr.sin_port = htons((short)port);
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr("49.235.145.135"); //此处更改epoll服务器地址
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
	{
		cout << "connect error" << endl;
		return nullptr;
	}

	cout << "succeed to connect server id : " << ip << " port : " << port << endl;

	return AddNewConn(sockfd);
}

Connection* ConnectionManager::AddNewConn(int32 fd)
{
	for (int32 i = 0; i < m_ConnectionVec.size(); i++)
	{
		Connection* pConn = m_ConnectionVec.at(i);
		if (!pConn->GetConnStatus())
		{
			std::cout << "new connid : " << pConn->GetConnectionID() << std::endl;

			pConn->SetSocket(fd);
		    AddEpollFd(fd, pConn, true);
			return pConn;
		}
	}
	return nullptr;
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
	nConnid -= 1;
	if (nConnid >= m_ConnectionVec.size())
	{
		return nullptr;
	}
	return m_ConnectionVec[nConnid];
}

void ConnectionManager::FreeConnByConnid(int32 nConnid)
{
	if (nConnid >= m_ConnectionVec.size())
	{
		return;
	}
	std::cout << "free nConnid : " << nConnid << std::endl;
	m_ConnectionVec[nConnid - 1]->Close();
}
