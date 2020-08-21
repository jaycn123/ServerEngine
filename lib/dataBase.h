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

	void    Read(char sql[] = (char*)"");
		    
	virtual void Insert(char sql[] = (char*)"") = 0;

	virtual void Delete(char sql[] = (char*)"") = 0;

	virtual void Modify(char sql[] = (char*)"") = 0;

	ChangeType      m_optype = CT_NoChange;

	uint32_t        m_dataNum = 0;

	uint64_t        m_gmid = 0;

	uint32_t        m_datalen = 0;

};

#endif