#ifndef DATABASE_H 
#define DATABASE_H

#include"type_define.h"
#include "mysql_control.h"

enum ChangeType
{
	CT_NoChange = 0,
	CT_Add = 1,
	CT_Delete =2,
	CT_Modify = 3,
	CT_Reverse = 4,
};

struct DB_Base
{
	DB_Base()
	{

	}
	virtual ~DB_Base()
	{

	}

	ChangeType ctype = CT_NoChange;
	MysqlControl* m_pMysql = nullptr;
	std::string m_sql = "";
	virtual void Insert() = 0;
	virtual void Delete() = 0;
	virtual void Modify() = 0;
	virtual void Reverse() = 0;
	virtual void Query(uint32_t parameter = 0) = 0;

	void Save();

	void Run(MysqlControl* pMysql);

	void Release();

	void Lock();

	void UnLock();

	std::mutex                              m_mutex;
};

#endif