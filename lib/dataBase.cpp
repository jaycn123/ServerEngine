#include "dataBase.h"
#include "serviceBase.h"

void DB_Base::Save()
{
	switch (m_optype)
	{
	case CT_NoChange:
		break;
	case CT_Add:
		Insert();
		break;
	case CT_Delete:
		Delete();
		break;
	case CT_Modify:
		Modify();
		break;
	case CT_Reverse:
		Reverse();
		break;
	default:
		break;
	}

	uint32_t datalen = m_sql.length() + sizeof(SqlData);
	SqlData* sqldata = (SqlData*)MemoryManager::GetInstancePtr()->GetFreeMemory(datalen);
	sqldata->m_len = datalen;
	sqldata->m_optype = m_optype;
	sqldata->m_datanum = m_dataNum;
	sqldata->m_gmid = m_gmid;
	strcpy(sqldata->m_sql, m_sql.c_str());

	ServiceBase::GetInstancePtr()->ChangeDB(sqldata);
	MemoryManager::GetInstancePtr()->FreeMemory(datalen, (char*)sqldata);
}

void DB_Base::Run(MysqlControl* pMysql)
{
	if (pMysql == nullptr)
	{
		return;
	}
	m_pMysql = pMysql;
	switch (m_optype)
	{
	case CT_NoChange:
		break;
	case CT_Add:
		Insert();
		break;
	case CT_Delete:
		Delete();
		break;
	case CT_Modify:
		Modify();
		break;
	case CT_Reverse:
		Reverse();
		break;
	default:
		break;
	}

}

void DB_Base::Release()
{

}

