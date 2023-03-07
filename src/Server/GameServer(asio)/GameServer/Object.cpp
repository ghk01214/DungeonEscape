#include "Object.h"

namespace game
{
	void CObject::Move(DIRECTION direction)
	{
		auto dir{ static_cast<int32_t>(direction) };

		if (dir & static_cast<int32_t>(DIRECTION::LEFT))
		{
			m_pos.x -= m_speed;
		}
		if (dir & static_cast<int32_t>(DIRECTION::RIGHT))
		{
			m_pos.x += m_speed;
		}
		if (dir & static_cast<int32_t>(DIRECTION::UP))
		{
			m_pos.y += m_speed;
		}
		if (dir & static_cast<int32_t>(DIRECTION::DOWN))
		{
			m_pos.y -= m_speed;
		}
		if (dir & static_cast<int32_t>(DIRECTION::FRONT))
		{
			m_pos.z += m_speed;
		}
		if (dir & static_cast<int32_t>(DIRECTION::BACK))
		{
			m_pos.z -= m_speed;
		}
	}

	void CObject::Rotate(ROTATION direction)
	{
		auto dir{ static_cast<int32_t>(direction) };

		if (dir & static_cast<int32_t>(ROTATION::X_INCREASE))
		{
			m_rotate.x += 2.f;
		}
		if (dir & static_cast<int32_t>(ROTATION::X_DECREASE))
		{
			m_rotate.x -= 2.f;
		}
		if (dir & static_cast<int32_t>(ROTATION::Y_INCREASE))
		{
			m_rotate.y += 2.f;
		}
		if (dir & static_cast<int32_t>(ROTATION::Y_DECREASE))
		{
			m_rotate.y -= 2.f;
		}
		if (dir & static_cast<int32_t>(ROTATION::Z_INCREASE))
		{
			m_rotate.z += 2.f;
		}
		if (dir & static_cast<int32_t>(ROTATION::Z_DECREASE))
		{
			m_rotate.z -= 2.f;
		}
	}
}