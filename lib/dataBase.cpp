#include "dataBase.h"
#include "serviceBase.h"

void DB_Base::Save()
{
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
	std::cout << m_sql << std::endl;
	ServiceBase::GetInstancePtr()->ChangeDB(m_sql);
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

}

void DB_Base::Release()
{

}

