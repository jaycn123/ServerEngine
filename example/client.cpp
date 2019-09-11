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
#include "../lib/type_define.h"
#include "../protoFiles/test.pb.h"
#include "../lib/netpack.h"

/*
#define NET_PACKET_DATA_SIZE 1024
#define NET_CODE 0x2766


enum NetMessage
{
	SENDDATA = 1,
};

struct  NetPacketHeader
{
	uint32          wDataSize = 0;
	uint32          wOpcode = SENDDATA;
	uint32          wCode = NET_CODE;
};

struct NetPacket
{
	NetPacketHeader     Header;
	char  pData[0];
};

*/

using namespace std;


#define BUFF_SIZE 170000000 //70000000

char m_sendBuff[BUFF_SIZE];
int m_buffsize = 0;
int m_sendoffindex = 0;

int sockfd = 0;

std::mutex m_mutex;

uint32_t tempCount = 100;

void sendMessage()
{
	static uint32_t i = 0;
	i++;
	//std::cout << "msg.Header.wDataSize : " << msg.Header.wDataSize << std::endl;

	for (;;)
	//for (uint32_t j = 0;j < 1; j++)
	{
		NetPacket msg;
		msg.Header.wOpcode = SENDDATA;
		msg.Header.wCode = NET_CODE;

		HeartBeatReq sendPack;
		sendPack.set_connid(i ++);

		char szBuff[102400] = { 0 };
		sendPack.ByteSize();
		//std::cout << sendPack.ByteSize() << std::endl;
		sendPack.SerializePartialToArray(szBuff, sendPack.GetCachedSize());
		memcpy(msg.pData, szBuff, sendPack.ByteSize());
		char temp[1024] = { 0 };
		memcpy(temp, szBuff, sendPack.ByteSize());
		bzero(szBuff, 102400);
		msg.Header.wDataSize = sendPack.GetCachedSize() + sizeof(NetPacketHeader);
		memcpy(szBuff, (char*)&msg, sendPack.GetCachedSize() + sizeof(NetPacketHeader));

		//std::cout << "msg.Header.wDataSize : " << msg.Header.wDataSize << "sendPack.connid : " << sendPack.connid() << std::endl;

		//AUTOMUTEX
		int wlen = send(sockfd, szBuff, msg.Header.wDataSize, 0);
		//std::cout << "send len  : " << wlen << std::endl;
		//sleep(1);
		//usleep(1000);
	}
}
#define BUFFER_SIZE 1024
#define RECV_BUF_SIZE 102400
char                    m_RecvBuf[RECV_BUF_SIZE];

uint32                  m_RecvOffIndex = 0;

uint32                  m_nRecvSize = 0;

uint32                  m_tempCount = 0;



bool DoReceiveEx()
{
	while (1)
	{
		int32 length = 0;
		if (RECV_BUF_SIZE == m_nRecvSize)
		{
			if ((m_nRecvSize - m_RecvOffIndex) > 0)
			{

				std::cout << " clean m_nRecvSize " << m_nRecvSize <<" m_RecvOffIndex "<< m_RecvOffIndex << std::endl;


				memmove(m_RecvBuf, m_RecvBuf + m_RecvOffIndex, m_nRecvSize - m_RecvOffIndex);
				m_nRecvSize = m_nRecvSize - m_RecvOffIndex;
				m_RecvOffIndex = 0;

			}
			else
			{

				std::cout << "clean" << std::endl;

				bzero(m_RecvBuf, RECV_BUF_SIZE);

				m_RecvOffIndex = 0;
				m_nRecvSize = 0;
			}
		}

		length = recv(sockfd, m_RecvBuf + m_nRecvSize, RECV_BUF_SIZE - m_nRecvSize, 0);

		std::cout << "recv len : " << length <<" RECV_BUF_SIZE - m_nRecvSize "<< RECV_BUF_SIZE - m_nRecvSize << std::endl;
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
					std::cout << "ssssssssssssssssssssssssssssssssss" << std::endl;
					break;
				}

				uint32 datalen = pHeader->wDataSize - sizeof(NetPacketHeader);

				char data[102400];
				bzero(data, 102400);
				memcpy(data, m_RecvBuf + m_RecvOffIndex + sizeof(NetPacketHeader), datalen);

				HeartBeatReq Req;
				Req.ParsePartialFromArray(data, datalen);

				std::cout << "recv Req : " << Req.connid() << std::endl;

	

				//sendMessage();

				m_RecvOffIndex += pHeader->wDataSize;
			}
		}
		else
		{
			if (length == 0)
			{
				std::cout << "length =0 " << "  m_RecvOffIndex " << m_RecvOffIndex << " m_nRecvSize " << m_nRecvSize << std::endl;
				return false;
			}
			if (errno == EAGAIN)
			{
				std::cout << "length : EAGAIN" << std::endl;
				return true;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	int on = 1;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "create socket fail" << endl;
		return -1;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	servaddr.sin_port = htons((short)8888);
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr("101.37.12.228"); //此处更改epoll服务器地址
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		cout << "connect error" << endl;
		return -1;
	}

	cout << "succeed to connect epoll server " << endl;

 	std::thread t(sendMessage);
 	t.detach();
	//sendMessage();
	char revcbuff[1024];
	bzero(revcbuff, 1024);

 	while(1)
 	{
		//std::cout << "DoReceiveEx" << std::endl;
		if (!DoReceiveEx())
		{
			//return 0;
		}
		usleep(3500);
		//sleep(1);
 	}
	close(sockfd);
	return 0;
	
}
