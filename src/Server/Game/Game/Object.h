#pragma once

namespace game
{
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
		CObject(Pos pos);
		CObject(float x, float y, float z);
		virtual ~CObject() = default;

		virtual void Init() {}

		virtual void Move(DIRECTION direction, float deltaTime);
		virtual void Rotate(ROTATION direction, float deltaTime);

		constexpr Pos GetPos() const { return m_pos; }
		constexpr Rotation GetRotation() const { return m_rotate; }

		void SetPos(Pos pos) { m_pos = pos; }
		void SetPos(float x, float y, float z) { SetPos(Pos{ x, y, z }); }
		void SetRotate(Rotation rotate) { m_rotate = rotate; }
		void SetRotate(float x, float y, float z) { SetRotate(Rotation{ x, y, z }); }

		void ChangeSpeed(float speed) { m_speed = speed; }

	protected:
		Pos m_pos;
		Rotation m_rotate;

		float m_speed = 200.f;
	};
}