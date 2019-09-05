#ifndef __TYPE_DEFINE_H__
#define __TYPE_DEFINE_H__

#include <iostream>
#include <assert.h>
#include <map>
#include <set>
#include <utility>
#include <queue>
#include <deque>
#include <vector>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <time.h>
#include <limits>
#include <memory>
#include <poll.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <errno.h>
#include <stddef.h>
#include <algorithm>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <sys/eventfd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/timerfd.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/uio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <google/protobuf/message.h>  //pb
#include <thread>
#include <functional>
#include <mutex>

using namespace std;
//using namespace std::placeholders;

typedef float f32;
typedef double f64;
typedef char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef int					BOOL;
typedef std::string         xstring;


typedef std::function<void(void)> func;

#define SERVER_IP "127.0.0.1"

//桥接后在同一网段即可选用以下地址
//#define CLIENT_IP "192.168.1.XXX"

// server port
//#define SERVER_PORT 8888
#define PORT 8888
//epoll 支持的最大并发量
#define EPOLL_SIZE 10000

//message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

// exit
#define EXIT "EXIT"

#define CAUTION "There is only one int the char room!"


#ifdef _DEBUG
#define HEART_BEAT_TIME		600
#else
#define HEART_BEAT_TIME		60
#endif

#define UPDATETIME			0.025
#define SECOND				1
#define MINUTE				60
#define HOUR				3600
#define ONE_DAY				86400

#define NowTime             uint64(time(0))

template <bool x> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> { enum {value = 1};};
template <int x> struct static_assert_test {};

#define JOIN_TEST(a,b) a##b

#define STATIC_ASSERT_CHECK(B) \
	typedef static_assert_test<sizeof(STATIC_ASSERTION_FAILURE<(bool)(B)>)> \
	JOIN_TEST(st_assert_test,__LINE__) __attribute__((unused))


#define DELETE(x) { if(x != NULL) {delete x; x = NULL;} }

#define CHECKERR_AND_RETURN(x)  { if ((x) == false) { ERROR("Check Error"); return;}  }

#define CHECKERR_AND_CONTINUE(x)  { if ((x) == false) { ERROR("Check Error"); continue;}  }

#define CHECKERR_AND_RETURN_VALUE(x,ret)  { if ((x) == false) { ERROR("Check Error"); return ret;}  }



#define AUTOMUTEX std::lock_guard<std::mutex> lock(m_mutex);

enum SendStatus
{
	SendComplete = 1,
	SendPart = 2,
	SendError = 3,
};

#endif
