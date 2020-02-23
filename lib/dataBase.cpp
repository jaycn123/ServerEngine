#include "dataBase.h"
#include "serviceBase.h"

void DB_Base::Save()
{
	ServiceBase::GetInstancePtr()->ChangeDB(this);
}

void DB_Base::Run(MysqlControl* pMysql)
{
	if (pMysql == nullptr)
	{
		return;
	}
	m_pMysql = pMysql;
	switch (ctype)
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
	m_mutex.lock();
	ctype = CT_NoChange;
	m_mutex.unlock();

}

void DB_Base::Release()
{
	m_mutex.lock();
	ctype = CT_Delete;
	m_mutex.unlock();
	ServiceBase::GetInstancePtr()->ChangeDB(this);
}

void DB_Base::Lock()
{
	m_mutex.lock();
}

void DB_Base::UnLock()
{
	m_mutex.unlock();
}
