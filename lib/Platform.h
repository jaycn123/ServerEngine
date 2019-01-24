#ifndef __PLATFORM_H__
#define __PLATFORM_H__



#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <string.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <list>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <unistd.h>

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef int					BOOL;
typedef std::string         xstring;

typedef float				FLOAT;
typedef double				DOUBLE;
typedef long long			int64;
typedef unsigned long long	uint64;
typedef char				BYTE;
typedef char				CHAR;
typedef int					SOCKET;
typedef int					HANDLE;

typedef std::string         xstring;


#define SERVER_IP "127.0.0.1"

//桥接后在同一网段即可选用以下地址
//#define CLIENT_IP "192.168.1.XXX"

// server port
#define SERVER_PORT 8888

//epoll 支持的最大并发量
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

// exit
#define EXIT "EXIT"

#define CAUTION "There is only one int the char room!"

#endif /* __PLATFORM_H__ */
