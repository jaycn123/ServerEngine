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

#define NET_PACKET_DATA_SIZE 1024
#define NET_CODE 0x2766


enum NetMessage
{
	SENDDATA = 1,
};

struct  NetPacketHeader
{
	uint32          wDataSize = 0;
	NetMessage      wOpcode = SENDDATA;
	uint32          wCode = NET_CODE;
	uint32          wConnId = 0;
};

struct NetPacket
{
	NetPacketHeader     Header;
	char  Data[1024];
};



using namespace std;


#define BUFF_SIZE 170000000 //70000000

char m_sendBuff[BUFF_SIZE];
int m_buffsize = 0;
int m_sendoffindex = 0;

void sendMessage(char *pdata, int len);
void sendMeaasge();
int sockfd = 0;
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

	servaddr.sin_port = htons((short)PORT);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //此处更改epoll服务器地址

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		cout << "connect error" << endl;
		return -1;
	}

	cout << "succeed to connect epoll server " << endl;

	NetPacket msg;
	msg.Header.wConnId = 1;
	msg.Header.wOpcode = SENDDATA;
	msg.Header.wCode = NET_CODE;


	HeartBeatReq sendPack;
	sendPack.set_connid(100);

	char szBuff[102400] = { 0 };
	std::cout << sendPack.ByteSize() << std::endl;
	sendPack.SerializePartialToArray(szBuff, sendPack.GetCachedSize());
	memcpy(msg.Data, szBuff, sendPack.ByteSize());
	char temp[1024] = { 0 };
	memcpy(temp, szBuff, sendPack.ByteSize());

	bzero(szBuff, 102400);
	msg.Header.wDataSize = sendPack.GetCachedSize() + sizeof(NetPacketHeader);
	memcpy(szBuff, (char*)&msg, sendPack.GetCachedSize() + sizeof(NetPacketHeader));

	std::cout << "msg.Header.wDataSize : " << msg.Header.wDataSize << std::endl;

	int wlen = send(sockfd, szBuff, msg.Header.wDataSize, 0);

	while (1)
	{
		int wlen = send(sockfd, szBuff, msg.Header.wDataSize, 0);
		sleep(1);
	}

	close(sockfd);
	return 0;
	
}
