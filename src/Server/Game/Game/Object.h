#pragma once

namespace game
{
	struct Vec3
	{
		float x;
		float y;
		float z;
	};

	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;
	};

	using Pos = Vec3;
	using Quat = Vec4;
	using Scale = Vec3;

	struct Trans
	{
		Pos p;
		Quat q;
		Scale s;
	};

	class CObject
	{
	public:
		CObject();
		CObject(Trans trans);
		CObject(Pos pos, Quat quat);
		CObject(float x, float y, float z);
		CObject(float x, float y, float z, float w);
		CObject(float px, float py, float pz, float qx, float qy, float qz, float qw);
		virtual ~CObject() = default;

		virtual void Init() {}

		virtual void Transform(uint8_t keyInput, server::KEY_STATE keyState);

		const Trans& GetTransform() const { return m_transform; }

		void SetTransform(Trans transform) { m_transform = transform; }
		void SetTransform(Pos pos, Quat quat) { SetTransform(Trans{ pos, quat }); }
		void SetPos(Pos pos) { m_transform.p = pos; }
		void SetPos(float x, float y, float z) { SetPos(Pos{ x, y, z }); }
		void SetQuat(Quat quat) { m_transform.q = quat; }
		void SetQuat(float x, float y, float z, float w) { SetQuat(Quat{ x, y, z, w }); }
		void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }
		void SetSpeed(float speed) { m_speed = speed; }
		void SetAngle(float angle) { m_angle = angle; }

	protected:
		Trans m_transform;

		float m_deltaTime;
		float m_speed;
		float m_angle;
	};
}