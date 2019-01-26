#ifndef CONNECTION_H
#define CONNECTION_H
#include <iostream>
#include "type_define.h"
#include "ThreadPool.h"
#include "Connection.h"

class ConnectionManager
{
	
public:
	ConnectionManager();

	~ConnectionManager();

	bool		CreteSocket(xstring& ip, int32 port);

	bool	    CreateEpollEvent();

	void        AddEpollFd(bool enable_et = false);

	void        AddEpollFd(int fd, bool enable_et);

	void        sendBroadcastmessage(int32 clientfd);

	int         setnonblocking(int sockfd);

	void        Run();

	void        SetConnectionNum(int32 nMaxCons);

	void        Init();

	void        Start();

	void        Close();

	bool        CanExit();

	bool		SendConnIDToClient(int32 fd, int32 connID);

protected:

	void        AddNewConn(int32 fd);

	Connection* GetConnByFd(int32 fd);

	Connection* GetConnByConnid(int32 nConnid);

	void        FreeConnByConnid(int32 nConnid);

private:

	int32 m_listener;

	int32 m_epfd;

	struct epoll_event m_events[EPOLL_SIZE];

	fivestar::ThreadPool m_threadPool;

	typedef std::function<void()> Looptask;

	Looptask m_Looptask;

	std::vector<Connection*>m_ConnectionVec;

	std::map<int32, int32>m_ConnectionMap;

};

#endif
