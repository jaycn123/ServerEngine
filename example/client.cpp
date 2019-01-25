//compile: g++ -g epoll_client.cpp -o epoll_client
//run: ./epoll_client
//
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../lib/netpack.h"

using namespace std;

#define PORT 9999
#define BUFF_SIZE 1 //70000000

char m_sendBuff[BUFF_SIZE];
int m_buffsize = 0;
int m_sendoffindex = 0;

void sendMessage(char *pdata, int len);
void sendMeaasge();
int sockfd;
int main(int argc, char* argv[])
{
	int on = 1;
	char buffer[512] = { 0 };
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "create socket fail" << endl;
		return -1;
	}
	cout << "succeed to create client socket fd " << sockfd << endl;

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	cout << "set socket reuse by etsockopt" << endl;

	servaddr.sin_port = htons((short)PORT);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("119.23.13.79"); //此处更改epoll服务器地址
	//127.0.0.1
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		cout << "connect error" << endl;
		return -1;
	}
	cout << "succeed to connect epoll server " << endl;

	char target[] = "The Author: tao_627@aliyun.com";
	memcpy(buffer, target, strlen(target));
	while (1)
	{
		sleep(300);
	}
	return 0;

	int i = 1;
	int numC = 5000000;
	while (numC > 0)
	{
		numC--;
		//i++;
		NetPacket msg;
		msg.Header.wConnId = 1;
		msg.Header.wOpcode = SENDDATA;
		msg.Header.wCode = NET_CODE;
		std::string tempstr = std::to_string(i);
		if (numC == 0)
		{
			tempstr = "10";
		}

		memcpy(msg.Data, tempstr.c_str(), tempstr.length());
		msg.Header.wDataSize = tempstr.length() + sizeof(NetPacketHeader);

		//memcpy(buffer, (char*)&msg, sizeof(NetPacketHeader) + tempstr.length());

		sendMessage((char*)&msg, msg.Header.wDataSize);

		/*
		int wlen = send(sockfd, buffer, sizeof(NetPacketHeader) + tempstr.length(), 0);
		if (wlen <= 0)
		{
			cout << " send data to server fail " << strerror(errno) << endl;
		}
		//cout << "send data to server on success, data: [" << wlen << "]" << endl;
		//sleep(1);
		if (numC % 100000 == 0)
		{
			cout << " numC :  " << numC << endl;
		}
		*/
	}
	/*
	for (int i = 0; i < 20; i++)
	{
		cout << "buffsize : " << m_buffsize << std::endl;

		while (m_sendoffindex < m_buffsize)
		{
			int wlen = send(sockfd, m_sendBuff + m_sendoffindex, m_buffsize, 0);
			if (wlen <= 0)
			{
				std::cout << "len : " << wlen << std::endl;
				cout << " send data to server fail " << strerror(errno) << endl;
				break;
			}
			m_sendoffindex += wlen;
			std::cout << "len : " << wlen << " : " << m_sendoffindex << std::endl;
		}
		m_sendoffindex = 0;
	}
	*/
	//sendMeaasge();
	while (1)
	{
		sendMeaasge();
	}
	close(sockfd);
	return 0;
	while (1)
	{
// 		memset(buffer, 0, sizeof(buffer));
// 
// 		int rlen = recv(sockfd, buffer, sizeof(buffer), 0);
// 		if (rlen <= 0)
// 			cout << " receive data from server fail " << strerror(errno) << endl;
// 		cout << "receive data from server on success, data: [" << buffer << "]" << endl;
		sendMeaasge();
	}
	

	return 0;
}

void sendMessage(char *pdata,int len)
{
	//if ((m_buffsize + len) < BUFF_SIZE)
	{
		memcpy(m_sendBuff + m_buffsize, pdata, len);
		m_buffsize += len;
	//	cout << "buffsize : " << m_buffsize << std::endl;
	}
}

void sendMeaasge()
{
	for (int i = 0; i < 1; i++)
	{
		cout << "buffsize : " << m_buffsize << std::endl;

		while (m_sendoffindex < m_buffsize)
		{
			int wlen = send(sockfd, m_sendBuff + m_sendoffindex, m_buffsize, 0);
			if (wlen <= 0)
			{
				std::cout << "len : " << wlen << std::endl;
				cout << " send data to server fail " << strerror(errno) << endl;
				break;
			}
			m_sendoffindex += wlen;
			std::cout << "len : " << wlen << " : " << m_sendoffindex << std::endl;
		}
		m_sendoffindex = 0;
	}
}