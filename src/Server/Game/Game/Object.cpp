#include "pch.h"
#include "Object.h"

namespace game
{
	CObject::CObject(Pos pos) :
		m_pos{ pos }
	{
	}

	CObject::CObject(float x, float y, float z) :
		m_pos{ x, y, z }
	{
	}

	void CObject::Move(DIRECTION direction, float deltaTime)
	{
		auto dir{ static_cast<int32_t>(direction) };

		// 이동방향에 따른 좌표 이동
		if (dir == static_cast<int32_t>(DIRECTION::LEFT))
		{
			m_pos.x -= m_speed * deltaTime;
		}
		if (dir == static_cast<int32_t>(DIRECTION::RIGHT))
		{
			m_pos.x += m_speed * deltaTime;
		}
		if (dir == static_cast<int32_t>(DIRECTION::UP))
		{
			m_pos.y += m_speed * deltaTime;
		}
		if (dir == static_cast<int32_t>(DIRECTION::DOWN))
		{
			m_pos.y -= m_speed * deltaTime;
		}
		if (dir == static_cast<int32_t>(DIRECTION::FRONT))
		{
			m_pos.z += m_speed * deltaTime;
		}
		if (dir == static_cast<int32_t>(DIRECTION::BACK))
		{
			m_pos.z -= m_speed * deltaTime;
		}

		std::cout << std::format("{}, {}\n", m_pos.x, m_pos.y);
	}

	void CObject::Rotate(ROTATION direction, float deltaTime)
	{
		auto dir{ static_cast<int32_t>(direction) };

		// 회전방향에 따른 회전
		if (dir == static_cast<int32_t>(ROTATION::X_INCREASE))
		{
			m_rotate.x += 2.f * deltaTime;
		}
		if (dir == static_cast<int32_t>(ROTATION::X_DECREASE))
		{
			m_rotate.x -= 2.f * deltaTime;
		}
		if (dir == static_cast<int32_t>(ROTATION::Y_INCREASE))
		{
			m_rotate.y += 2.f * deltaTime;
		}
		if (dir == static_cast<int32_t>(ROTATION::Y_DECREASE))
		{
			m_rotate.y -= 2.f * deltaTime;
		}
		if (dir == static_cast<int32_t>(ROTATION::Z_INCREASE))
		{
			m_rotate.z += 2.f * deltaTime;
		}
		if (dir == static_cast<int32_t>(ROTATION::Z_DECREASE))
		{
			m_rotate.z -= 2.f * deltaTime;
		}
	}
}