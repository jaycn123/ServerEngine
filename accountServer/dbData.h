#include "../lib/dataBase.h"
#include "../lib/type_define.h"

struct DB_Account : public DB_Base
{
	DB_Account(uint64_t gmid = 0)
	{
		m_dataNum = DTN_Account;

		m_gmid = gmid;

		m_datalen = sizeof(DB_Account);
	}

	void Insert(char sql[] = (char*)"")
	{
		auto str = "INSERT INTO account (`id`, `user`, `password`) VALUES('" + std::to_string(id) + "','" + user + "', '" + password + " ');";
		strcpy(sql, str.c_str());
	}

	void Delete(char sql[] = (char*)"")
	{
		auto str = "DELETE FROM account WHERE id = " + std::to_string(id) + ";";
		strcpy(sql, str.c_str());
	}

	void Modify(char sql[] = (char*)"")
	{
		auto str = "Update account set `password` = '" + std::string(password) + "' WHERE `user` = '" + user + "';";
		strcpy(sql, str.c_str());
	}

	uint32_t id = 0;
	char user[64] = { 0 };
	char password[64] = { 0 };
};
