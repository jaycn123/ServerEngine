#ifndef  PLAYER_H
#define  PLAYER_H

#include "../lib/type_define.h"


class Player //: public DB_Player
{

public:

	uint32		GetRoleID();

	uint32		GetConnID();

	void		SetConnID(uint32 connid);

	void		SetName(std::string& name);

	std::string GetName();

private: 

	uint32 m_connID = 0;

};

#endif