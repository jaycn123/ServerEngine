#include "dataBase.h"
#include "serviceBase.h"

void DB_Base::Save()
{
	ServiceBase::GetInstancePtr()->SendDataToDB((char*)this, m_datalen);
}

void DB_Base::Read(char sql[] /*= ""*/)
{
	switch (m_optype)
	{
	case CT_Add:
		Insert(sql);
		break;
	case CT_Delete:
		Delete(sql);
		break;
	case CT_Modify:
		Modify(sql);
		break;
	default:
		break;
	}

	return;
}
