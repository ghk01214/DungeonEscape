#pragma once

//#include "Object.h"

namespace game
{
	struct Pos;
	struct Quat;
	struct Trans;

	class CObject;

	class CPlayer : public CObject
	{
	public:
		CPlayer();
		CPlayer(Trans trans);
		CPlayer(Pos pos, Quat quat);
		CPlayer(float x, float y, float z);
		CPlayer(float x, float y, float z, float w);
		CPlayer(float px, float py, float pz, float qx, float qy, float qz, float qw);
		~CPlayer();

		void Init() override;

		void Transform(uint8_t keyInput, server::KEY_STATE keyState, float deltaTime) override;
		void Jump();

		constexpr int32_t GetAniIndex() const { return m_aniIndex; }
		constexpr float GetAniFrame() const { return m_aniFrame; }

		constexpr bool IsJumping() const { return m_jump; }

		void SetAniIndex(int32_t index) { m_aniIndex = index; }
		void SetAniFrame(float aniFrame) { m_aniFrame = aniFrame; }

	private:
		int32_t m_aniIndex;
		float m_aniFrame;

		bool m_jump;
		float m_jumpSpeed;
	};
}