#include "pch.h"
#include "Object.h"

namespace game
{
	CObject::CObject() :
		m_transform{ Pos{ 0.f, 0.f, 0.f }, Quat{ 0.f, 0.f, 0.f, 0.f } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(Trans trans) :
		m_transform{ trans },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(Pos pos, Quat quat) :
		m_transform{ pos, quat },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float x, float y, float z) :
		m_transform{ Pos{ x, y, z }, Quat{ 0.f, 0.f, 0.f, 0.f } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float x, float y, float z, float w) :
		m_transform{ Pos{ 0.f, 0.f, 0.f }, Quat{ x, y, z, w } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float px, float py, float pz, float qx, float qy, float qz, float qw) :
		m_transform{ Pos{ px, py, pz }, Quat{ qx, qy, qz, qw } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	void CObject::Transform(DIRECTION direction, ROTATION quaternion, float deltaTime)
	{
		auto dir{ static_cast<uint8_t>(direction) };
		auto quat{ static_cast<uint8_t>(quaternion) };

		// 이동방향에 따른 좌표 이동
		if (dir == static_cast<uint8_t>(DIRECTION::LEFT))
		{
			m_transform.p.x -= m_speed * deltaTime;
		}
		if (dir == static_cast<uint8_t>(DIRECTION::RIGHT))
		{
			m_transform.p.x += m_speed * deltaTime;
		}
		if (dir == static_cast<uint8_t>(DIRECTION::UP))
		{
			m_transform.p.y += m_speed * deltaTime;
		}
		if (dir == static_cast<uint8_t>(DIRECTION::DOWN))
		{
			m_transform.p.y -= m_speed * deltaTime;
		}
		if (dir == static_cast<uint8_t>(DIRECTION::FRONT))
		{
			m_transform.p.z += m_speed * deltaTime;
		}
		if (dir == static_cast<uint8_t>(DIRECTION::BACK))
		{
			m_transform.p.z -= m_speed * deltaTime;
		}

		// 회전방향에 따른 회전
		if (quat == static_cast<uint8_t>(ROTATION::X_INCREASE))
		{
			m_transform.q.x += m_angle * deltaTime;
		}
		if (quat == static_cast<uint8_t>(ROTATION::X_DECREASE))
		{
			m_transform.q.x -= m_angle * deltaTime;
		}
		if (quat == static_cast<uint8_t>(ROTATION::Y_INCREASE))
		{
			m_transform.q.y += m_angle * deltaTime;
		}
		if (quat == static_cast<uint8_t>(ROTATION::Y_DECREASE))
		{
			m_transform.q.y -= m_angle * deltaTime;
		}
		if (quat == static_cast<uint8_t>(ROTATION::Z_INCREASE))
		{
			m_transform.q.z += m_angle * deltaTime;
		}
		if (quat == static_cast<uint8_t>(ROTATION::Z_DECREASE))
		{
			m_transform.q.z -= m_angle * deltaTime;
		}

		std::cout << std::format("{}, {}, {}\n", m_transform.p.x, m_transform.p.y, m_transform.p.z);
		std::cout << std::format("{}, {}, {}, {}\n", m_transform.q.x, m_transform.q.y, m_transform.q.z, m_transform.q.w);
	}
}