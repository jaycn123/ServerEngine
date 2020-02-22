#include "player.h"

uint32 Player::GetRoleID()
{
	return m_roleID;
}

uint32 Player::GetConnID()
{
	return m_connID;
}

void Player::SetConnID(uint32 connid)
{
	m_connID = connid;
}

void Player::SetName(std::string& name)
{
	m_name = name;
}

std::string Player::GetName()
{
	return m_name;
}

