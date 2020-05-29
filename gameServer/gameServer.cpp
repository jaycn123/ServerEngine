#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "gameServer.h"
#include "../lib/Log.h"
#include "../lib/configFile.h"
#include "../protoFiles/msgId.pb.h"

GameServer::GameServer(void)
{
	
}

GameServer::~GameServer(void)
{

}

GameServer* GameServer::GetInstancePtr()
{
	static GameServer gameService;
	return &gameService;
}

void GameServer::Init()
{
	if (!CLog::GetInstancePtr()->StartLog("gameServer", "log"))
	{
		std::cout << "error can not create log" << std::endl;
	}

	CLog::GetInstancePtr()->LogError("---------server start-----------");

	if (!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		CLog::GetInstancePtr()->LogError("配制文件加载失败!");
		return;
	}

	CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("game_log_level"));

	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_port");
	uint32_t maxcon = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_max_con");
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("game_svr_ip");

	std::cout << "port : " << port << " maxcon : " << maxcon << std::endl;

	ServiceBase::GetInstancePtr()->StartNetWork(ip, port, maxcon,this);


	std::string mysqlip = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_ip");
	uint32_t mysqlport = CConfigFile::GetInstancePtr()->GetIntValue("mysql_game_svr_port");
	std::string dbname = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_db_name");

	std::string user = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_user");
	std::string password = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_pwd");

	m_pMysql = new MysqlControl();
	if (m_pMysql->RealConnect(mysqlip.c_str(), user.c_str(), password.c_str(), dbname.c_str()) == false)
	{
		CLog::GetInstancePtr()->LogError("conncetion mysql fail!");
		return;
	}
	ServiceBase::GetInstancePtr()->SetMysqlControl(m_pMysql);
	PlayerManager::GetInstancePtr()->Init(m_pMysql);

}

void GameServer::Uninit()
{

}

void GameServer::Run()
{
	ServiceBase::GetInstancePtr()->Run();
}

bool GameServer::DispatchPacket(CNetPacket* pNetPacket)
{
	return true;
}

void GameServer::OnSecondTimer()
{
	if (!m_GateServerConnId)
	{
		ConnectionGate();
	}
}

void GameServer::OnCloseConnect(Connection* pConn)
{

}

void GameServer::OnNewConnect(Connection* pConnection)
{
	if (m_GateServerConnId == pConnection->m_ConnID)
	{
		std::cout << "m_GateServerConnId : " << m_GateServerConnId << std::endl;
	}
}

bool GameServer::ConnectionGate()
{
	std::string ip = CConfigFile::GetInstancePtr()->GetStringValue("gate_svr_ip");
	std::string areaname = CConfigFile::GetInstancePtr()->GetStringValue("areaname");
	uint32_t areaid = CConfigFile::GetInstancePtr()->GetIntValue("areaid");
	uint32_t port = CConfigFile::GetInstancePtr()->GetIntValue("gate_svr_port");
	auto pConn = ConnectionManager::GetInstancePtr()->ConnectionToServer(ip, port);
	if (!pConn)
	{
		CLog::GetInstancePtr()->LogError("connect gate fail ! ");
		return false;
	}

	m_GateServerConnId = pConn->m_ConnID;

	Msg_Connetc_Info Req;
	Req.set_stype(ServerType::ST_Game);
	Req.set_name(areaname);
	Req.set_serverid(areaid);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_GateServerConnId, MessageID::MSGID_CONNETC_INFO, Req);

	return true;
}


void GameServer::RegisterMsg()
{
	ServiceBase::GetInstancePtr()->RegisterMsg(MessageID::MSGID_CONNETC_NOTICE, std::bind(&GameServer::OnConnectNotice, this, std::placeholders::_1));
}

void GameServer::InitMsg()
{
	GameServer::GetInstancePtr()->RegisterMsg();
	PlayerManager::GetInstancePtr()->RegisterMsg();
}

void GameServer::OnConnectNotice(CNetPacket* pNetPacket)
{
	Msg_Connetc_Notice Req;
	Req.ParsePartialFromArray(pNetPacket->m_pData, pNetPacket->m_len);
	std::cout << Req.connid() << "  : "<<Req.status() << std::endl;
}

void GameServer::testSendProtobuf(uint32 connid,uint32_t tempcount)
{
	HeartBeatReq Req;
	Req.set_connid(tempcount);
	ServiceBase::GetInstancePtr()->SendMsgProtoBuf(connid, 1, Req);
}

int main()
{
	GameServer::GetInstancePtr()->Init();
	GameServer::GetInstancePtr()->Run();
	std::cout << "exit-----------------------------------------" << std::endl;
	return 0;
}
