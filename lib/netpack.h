#pragma once

#define NET_PACKET_DATA_SIZE 1024
#define NET_CODE 0x2766
#define MAXNAME 512

enum NetMessage
{
	SENDDATA = 1,
};

struct  NetPacketHeader
{
	uint32_t            wDataSize = 0;
	NetMessage          wOpcode = SENDDATA;
	uint32_t            wCode = NET_CODE;
	uint32_t            wConnId = 0;
};

struct NetPacket
{
	NetPacketHeader     Header;               
	char  Data[1024];
};

