#include"type_define.h"

enum ChangeType
{
	CT_NoChange = 0,
	CT_Add = 1,
	CT_Delete =2,
	CT_Modify = 3,
};

struct DB_Base
{
	ChangeType ctype = CT_NoChange;
	virtual void Insert() = 0;
	virtual void Delete() = 0;
	virtual void Modify() = 0;
	virtual void Query(uint32_t parameter = 0) = 0;
};
