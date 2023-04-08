#pragma once

namespace game
{
	struct Pos
	{
		float x;
		float y;
		float z;
	};

	struct Quat
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct Trans
	{
		Pos p;
		Quat q;
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

		virtual void Transform(uint8_t keyInput, server::KEY_STATE keyState, float deltaTime);

		const Trans GetTransform() const { return m_transform; }

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