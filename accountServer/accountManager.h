#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "../lib/messageQueue.h"
#include "../lib/configFile.h"
#include "../lib/Connection.h"
#include "dbData.h"
#include "../lib/mysql_control.h"


class AccountManager
{
private:

	AccountManager(void);

	virtual ~AccountManager(void);

public:

	static AccountManager* GetInstancePtr();

	bool		Init(MysqlControl* pMysql);

	void		Uninit();

	void        InitMsg();

	void        OnSecondTimer();

		
public:

	void        OnLoginAccount(CNetPacket* pNetPacket);

	void        OnCreateAccount(CNetPacket* pNetPacket);

protected:

	bool          GetAllAccount();
	 
	DB_Account*   AddAccount(int32_t id , std::string& user, std::string& password);

private:

	MysqlControl* m_pMysql = nullptr;

	std::map<std::string, DB_Account*> m_accountMap;
};

#endif
