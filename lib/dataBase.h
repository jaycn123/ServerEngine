#ifndef DATABASE_H 
#define DATABASE_H

#include"type_define.h"
#include "mysql_control.h"

enum ChangeType
{
	CT_NoChange = 0,
	CT_Add     = 1,
	CT_Modify  = 2,
	CT_Delete  = 3,
	CT_Reverse = 4,
};

enum DB_DataNum
{
	DTN_Start = 0,
	DTN_Account = 1,
	DTN_Max,
};


struct DB_Base
{
	DB_Base()
	{

	}

	virtual ~DB_Base()
	{

	}

	void    Save();
		    
	void    Run(MysqlControl* pMysql);
		    
	void    Release();

	virtual void Insert() = 0;

	virtual void Delete() = 0;

	virtual void Modify() = 0;

	virtual void Reverse() = 0;

	virtual void Query(uint32_t parameter = 0) = 0;

	ChangeType      m_optype = CT_NoChange;

	uint32_t        m_dataNum = 0;

	MysqlControl*   m_pMysql = nullptr;

	std::string     m_sql = "";

	uint64_t        m_gmid = 0;

	uint64_t        m_create_time = 0;

};

#endif