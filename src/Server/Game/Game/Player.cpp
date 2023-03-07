#include "pch.h"
#include "Object.h"
#include "Player.h"

namespace game
{
	CPlayer::CPlayer() :
		m_accessID{ -1 },
		m_connected{ false }
	{
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Init(int32_t id)
	{
		m_accessID = id;
	}
}