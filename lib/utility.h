//utility.h
#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

// 选用list方便删除sockfd
list<int> clients_list;


// ip
#define SERVER_IP "127.0.0.1"
//#define SERVER_IP "123.207.232.160"

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



//设置sockfd,pipefd非阻塞
int setnonblocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
    return 0;
}

/**
  * @param epollfd: epoll handle
  * @param fd: socket descriptor
  * @param enable_et : enable_et = true, epoll use ET; otherwise LT
**/
void addfd( int epollfd, int fd, bool enable_et )
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;     //输入触发epoll-event
    if( enable_et )
        ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
 //   printf("fd added to epoll!\n\n");
}

//将服务器收到的clientfd的消息进行广播
int sendBroadcastmessage(int clientfd)
{

    char buf[BUF_SIZE], message[BUF_SIZE];
    //清零
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    printf("read from client(clientID = %d)\n", clientfd);
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    if(len == 0)  // len = 0 client关闭了连接
    {
        close(clientfd);
        clients_list.remove(clientfd); //list删除fd
        printf("ClientID = %d closed.\n now there are %d client in the char room\n", clientfd, (int)clients_list.size());

    }
    else  //进行广播
    {
        if(clients_list.size() == 1) {
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }

        sprintf(message, SERVER_MESSAGE, clientfd, buf);

        list<int>::iterator it;
        for(it = clients_list.begin(); it != clients_list.end(); ++it) {
           if(*it != clientfd){
                if( send(*it, message, BUF_SIZE, 0) < 0 ) { perror("error"); exit(-1);}
           }
        }
    }
    return len;
}
#endif // UTILITY_H_INCLUDED
