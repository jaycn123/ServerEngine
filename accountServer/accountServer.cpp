#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "accountServer.h"
#include "../lib/Log.h"
#include "../protoFiles/msgId.pb.h"


AccountServer::AccountServer(void)
{
	
}

AccountServer::~AccountServer(void)
{

}

AccountServer* AccountServer::GetInstancePtr()
{
	static AccountServer cGameService;
	return &cGameService;
}

void AccountServer::Init()
{
	if (!CLog::GetInstancePtr()->StartLog("gateServer", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	CLog::GetInstancePtr()->LogError((char*)("---------server start-----------"));

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("配制文件加载失败!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("gate_log_level"));

	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("account_svr_port");
	uint32_t maxcon = CConfigFile::GetInstancePtr()->GetIntValue("account_svr_max_con");
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("account_svr_ip");

	std::cout << "port : " << port << " maxcon : "<< maxcon << std::endl;


	if (ServiceBase::GetInstancePtr()->StartNetWork(ip, port, maxcon, this) == false)
	{
		CLog::GetInstancePtr()->LogError("listen fail!");
		return;
	}

	std::string mysqlip = CConfigFile::GetInstancePtr()->GetStringValue("mysql_acc_svr_ip");
	uint32_t mysqlport = CConfigFile::GetInstancePtr()->GetIntValue("mysql_acc_svr_port");
	std::string dbname = CConfigFile::GetInstancePtr()->GetStringValue("mysql_acc_svr_db_name");

	std::string user = CConfigFile::GetInstancePtr()->GetStringValue("mysql_acc_svr_user");
	std::string password = CConfigFile::GetInstancePtr()->GetStringValue("mysql_acc_svr_pwd");

	m_pMysql = new MysqlControl();
	if (m_pMysql->RealConnect(mysqlip.c_str(), user.c_str(), password.c_str(), dbname.c_str()) == false)
	{
		CLog::GetInstancePtr()->LogError("conncetion mysql fail!");
		return;
	}
	ServiceBase::GetInstancePtr()->SetMysqlControl(m_pMysql);
	AccountManager::GetInstancePtr()->Init(m_pMysql);
}

void AccountServer::Uninit()
{

}

void AccountServer::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool AccountServer::DispatchPacket(CNetPacket* pNetPacket)
{

}

void AccountServer::OnSecondTimer()
{
	AccountManager::GetInstancePtr()->OnSecondTimer();

	if (m_GateServerConnID == 0)
	{
		ConnectionGate();
	}

	if (m_GateServerConnID == 0)
	{
		ConnectionGame();
	}
}

bool AccountServer::ConnectionGame()
{
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("game_svr_ip");
	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_port");
	auto pConn = ConnectionManager::GetInstancePtr()->ConnectionToServer(ip, port);
	if (!pConn)
	{
		CLog::GetInstancePtr()->LogError("connect game fail ! ");
		return false;
	}

	m_GameServerConnID = pConn->m_ConnID;

	Msg_Connetc_Info Req;
	Req.set_stype(ServerType::ST_Account);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GameServerConnID, MessageID::MSGID_CONNETC_INFO, Req);

	return true;
}

bool AccountServer::ConnectionGate()
{
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("gate_svr_ip");
	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("gate_svr_port");
	auto pConn = ConnectionManager::GetInstancePtr()->ConnectionToServer(ip, port);
	if (!pConn)
	{
		CLog::GetInstancePtr()->LogError("connect gate fail ! ");
		return false;
	}

	m_GateServerConnID = pConn->m_ConnID;

	Msg_Connetc_Info Req;
	Req.set_stype(ServerType::ST_Account);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GateServerConnID, MessageID::MSGID_CONNETC_INFO, Req);

	return true;
}

void AccountServer::OnCloseConnect(Connection* pConnection)
{

}

void AccountServer::OnNewConnect(Connection* pConnection)
{

}

void AccountServer::InitMsg()
{
	AccountManager::GetInstancePtr()->InitMsg();
}

bool AccountServer::OnForwardNetPack(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	std::cout << "OnMsgWatchHeartBeatReq : " << Req.connid() << std::endl;

	//std::cout << "MsgFrom : " << pNetPacket->m_connId << std::endl << std::endl;

	testSendProtobuf(pNetPacket->m_connId, Req.connid());
	return true;
}

bool AccountServer::OnMsgWatchHeartBeatReq(CNetPacket* pNetPacket)
{
	HeartBeatReq Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);

	testSendProtobuf(pNetPacket->m_connId, Req.connid());
}

void AccountServer::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	AccountServer::GetInstancePtr()->Init();
	AccountServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
