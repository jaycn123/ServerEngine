#ifndef NETPACK_H
#define NETPACK_H

#include "type_define.h"

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

#endif



