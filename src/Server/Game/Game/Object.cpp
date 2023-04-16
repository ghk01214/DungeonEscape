#include "pch.h"
#include "Object.h"

namespace game
{
	CObject::CObject() :
		m_transform{ PosOLD{ 0.f, 0.f, 0.f }, QuatOLD{ 0.f, 0.f, 0.f, 0.f } },
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

	CObject::CObject(PosOLD pos, QuatOLD quat) :
		m_transform{ pos, quat },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float x, float y, float z) :
		m_transform{ PosOLD{ x, y, z }, QuatOLD{ 0.f, 0.f, 0.f, 0.f } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float x, float y, float z, float w) :
		m_transform{ PosOLD{ 0.f, 0.f, 0.f }, QuatOLD{ x, y, z, w } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	CObject::CObject(float px, float py, float pz, float qx, float qy, float qz, float qw) :
		m_transform{ PosOLD{ px, py, pz }, QuatOLD{ qx, qy, qz, qw } },
		m_speed{ 200.f },
		m_angle{ 2.f },
		m_deltaTime{ 0.f }
	{
	}

	void CObject::Transform(uint8_t keyInput, server::KEY_STATE keyState, float deltaTime)
	{
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

		}

		std::cout << std::format("{}, {}, {}\n", m_transform.p.x, m_transform.p.y, m_transform.p.z);
		std::cout << std::format("{}, {}, {}, {}\n", m_transform.q.x, m_transform.q.y, m_transform.q.z, m_transform.q.w);
	}
}