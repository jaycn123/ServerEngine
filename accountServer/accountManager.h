#ifndef CHATSERVER_H
#define CHATSERVER_H

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
	
public:

protected:

	bool       GetAllAccount();

private:

	MysqlControl* m_pMysql = nullptr;

	std::map<std::string, DB_Account> m_accountMap;
};

#endif
