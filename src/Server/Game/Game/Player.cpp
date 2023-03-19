#include "pch.h"
#include "Object.h"
#include "Player.h"

namespace game
{
	CPlayer::CPlayer()
	{
	}

	CPlayer::CPlayer(Pos pos)
	{
		m_pos = pos;
	}

	CPlayer::CPlayer(float x, float y, float z)
	{
		m_pos.x = x;
		m_pos.y = y;
		m_pos.z = z;
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Init()
	{
	}
}