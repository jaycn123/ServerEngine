#include "../lib/dataBase.h"
#include "../lib/type_define.h"

struct DB_Account : public DB_Base
{
	uint32_t id = 0;
	std::string user = "";
	std::string password = "";

	void Insert()
	{
		m_mutex.lock();
		m_sql = "INSERT INTO account (`id`, `user`, `password`) VALUES('" + std::to_string(id) + "','" + user + "', '" + password + " ');";
		m_mutex.unlock();
		m_pMysql->Query(m_sql);
	}

	void Delete()
	{
		m_sql = "DELETE FROM account WHERE id = " + std::to_string(id) + ";";
		m_pMysql->Query(m_sql);
	}

	void Modify()
	{
		m_mutex.lock();
		m_sql = "Update account set `password` = '" + password + "' WHERE `user` = '" + user + "';";
		m_mutex.unlock();
		m_pMysql->Query(m_sql);
	}

	void Reverse()
	{
		m_mutex.lock();
		m_sql = "SELECT * FROM account where id = " + std::to_string(id) + ";";
		DoubleDArray<Field> arr;
		m_pMysql->QueryAndStore(m_sql);
		m_pMysql->GetAllResult(arr);
		std::map<std::string, int> fieldMap = m_pMysql->GetField();
		for (size_t i = 0; i < arr.GetRowCount(); i++)
		{
			user = arr.GetValue(i, fieldMap["user"]).GetString();
			password = arr.GetValue(i, fieldMap["password"]).GetString();
		}
		m_mutex.unlock();

	}

	void Query(uint32_t parameter)
	{

	}
};
