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

	CPlayer::CPlayer(PosOLD pos, QuatOLD quat) :
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

	void CPlayer::Transform(uint8_t keyInput, server::KEY_STATE keyState, float deltaTime)
	{
		// 키보드의 현재 상태(DOWN, PRESS, UP)
		auto ks{ static_cast<uint8_t>(keyState) };

		if (keyInput == 0x0)
			return;

		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::LEFT)) != 0)
		{
			m_transform.p.x -= m_speed * deltaTime;
		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::RIGHT)) != 0)
		{
			m_transform.p.x += m_speed * deltaTime;
		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::UP)) != 0)
		{
			m_transform.p.y += m_speed * deltaTime;
		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::DOWN)) != 0)
		{
			m_transform.p.y -= m_speed * deltaTime;
		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::SPACE)) != 0)
		{
			Jump();
		}

		std::cout << std::format("{}, {}, {}\n", m_transform.p.x, m_transform.p.y, m_transform.p.z);
		std::cout << std::format("{}, {}, {}, {}\n", m_transform.q.x, m_transform.q.y, m_transform.q.z, m_transform.q.w);
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