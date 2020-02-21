#ifndef SERVICE_BASE_H
#define SERVICE_BASE_H

#include "messageQueue.h"
#include "ConnectionManager.h"
#include "type_define.h"

#define MAXPACKNUM 9999

// #define PROCESS_MESSAGE_ITEMEX(dwMsgID, Func) \
// 		case dwMsgID:{\
// 		if(Func(pNetPacket)){return true;}}brea

#define PROCESS_MESSAGE_ITEMEX(dwMsgID, Func) \
		case dwMsgID:{\
		Func(pNetPacket);}break;


class ServiceBase : public IDataHandler
{
	typedef std::function<void(CNetPacket*)> msgfunc;

protected:

	ServiceBase(void);

	virtual ~ServiceBase(void);

public:

	static ServiceBase* GetInstancePtr();

	bool   StartNetWork(std::string& ip, uint32 port, uint32 maxConnNum, IPacketDispatcher* pDispather);

	//不需要监听端口  测试作为客户端用
	bool   InitClientConn(uint32 maxConnNum, IPacketDispatcher* pDispather);

	bool   Run();

	bool   StopNetWork();
	
	bool   AddNetPackToQueue(CNetPacket* pData);

	bool   SendMsgProtoBuf(uint32 dwConnID, uint32 dwMsgID,const google::protobuf::Message& pdata);

	bool   SendMsgProtoBuf(uint32 dwConnID,uint32_t sconnid, uint32 dwMsgID, const google::protobuf::Message& pdata);

	bool   SendDataByConnID(uint32 connid, CNetPacket* pPack);

	void   OnCloseConnect(Connection* pConnection);

	void   OnNewConnect(Connection* pConnection);

	void   RegisterMsg(uint32_t msgid, msgfunc fp);


protected:

	void   StartThreadParsing();

	void   ParsingLoop();

	void   ParsingNetPack();

	bool   ExecutionMsg(CNetPacket* pNetPack);

private:

	IPacketDispatcher *            m_pPacketDispatcher;

	std::queue<CNetPacket*>        m_NetPackQueue;

	std::mutex                     m_mutex;

	uint32                         m_checkConnStatus = 0;

	uint64_t                       m_tickCount = 0;

	std::map <uint32, std::vector<msgfunc>> m_msgFuncMap;

};


#endif /*__SERVICE_BASE_H__*/