#pragma once

#include <Packet.h>

namespace game
{
	SKIP_TCP_NAMESPACE;

	struct Pos
	{
		float x;
		float y;
		float z;
	};

	using Rotation = Pos;

	class CObject
	{
	public:
		CObject() = default;
		virtual ~CObject() = default;

		virtual void Init(uint32_t id) {}

		virtual void Move(DIRECTION direction);
		virtual void Rotate(ROTATION direction);

		constexpr Pos GetPos() const { return m_pos; }
		constexpr Rotation GetRotate() const { return m_rotate; }

		void SetPos(Pos pos) { m_pos = pos; }
		void SetPos(float x, float y, float z) { SetPos(Pos{ x, y, z }); }
		void SetRotation(Rotation rotate) { m_rotate = rotate; }
		void SetRotation(float x, float y, float z) { SetPos(Rotation{ x, y, z }); }
		
		void ChangeSpeed(float speed) { m_speed = speed; }

	protected:
		Pos m_pos;
		Rotation m_rotate;

		float m_speed;
	};
}