#include "../lib/ConnectionManager.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"
#include "AccountManager.h"
#include "../lib/Log.h"

AccountManager::AccountManager(void)
{
	
}

AccountManager::~AccountManager(void)
{

}

AccountManager* AccountManager::GetInstancePtr()
{
	static AccountManager cGameService;
	return &cGameService;
}

bool AccountManager::Init(MysqlControl* pMysql)
{
	if (pMysql == nullptr)
	{
		return false;
	}

	m_pMysql = pMysql;

	GetAllAccount();

	return true;
}

void AccountManager::Uninit()
{

}


void AccountManager::InitMsg()
{
	
}

bool AccountManager::GetAllAccount()
{

}
