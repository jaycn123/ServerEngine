#include "../lib/dataBase.h"
#include "../lib/type_define.h"

struct DB_Account : public DB_Base
{
	uint32_t id;
	char user[64] = { 0 };
	char passwd[64] = { 0 };

	void Insert()
	{

	}

	void Delete()
	{

	}

	void Modify()
	{

	}

	void Query(uint32_t parameter)
	{
		std::string sql = "SELECT * FROM account";
		if (parameter != 0)
		{
			sql += ("WHERE id = " + std::to_string(parameter));
		}
	}
};
