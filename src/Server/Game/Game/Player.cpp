#include "pch.h"
#include "Object.h"
#include "Player.h"

namespace game
{
	CPlayer::CPlayer() :
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::CPlayer(Trans trans) :
		CObject{ trans },
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::CPlayer(Pos pos, Quat quat) :
		CObject{ pos, quat },
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::CPlayer(float x, float y, float z) :
		CObject{ x, y, z },
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::CPlayer(float x, float y, float z, float w) :
		CObject{ x, y, z, w },
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::CPlayer(float px, float py, float pz, float qx, float qy, float qz, float qw) :
		CObject{ px, py, pz, qx, qy, qz, qw },
		m_aniIndex{ 0 },
		m_aniFrame{ 0.f },
		m_jump{ false },
		m_jumpSpeed{ 4.f }
	{
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Init()
	{
	}

	void CPlayer::Jump()
	{
		if (m_jump == false)
			m_jump = true;

		if (m_transform.p.y + m_jumpSpeed * m_deltaTime < 0.f)
		{
			m_jump = false;
			m_transform.p.y = 0.f;
			m_jumpSpeed = 4.f;

			return;
		}

		m_transform.p.y += m_jumpSpeed * m_deltaTime;
		m_jumpSpeed -= GRAVITY * m_deltaTime;
	}
}