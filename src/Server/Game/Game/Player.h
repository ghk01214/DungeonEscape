#pragma once

namespace game
{
	class CObject;

	class CPlayer : public CObject
	{
	public:
		CPlayer();
		CPlayer(Pos pos);
		CPlayer(float x, float y, float z);
		~CPlayer();

		void Init() override;

		constexpr int32_t GetAniIndex() const { return m_aniIndex; }
		constexpr float GetAniFrame() const { return m_aniFrame; }

		void SetAniIndex(int32_t index) { m_aniIndex = index; }
		void SetAniFrame(float aniFrame) { m_aniFrame = aniFrame; }

	private:
		int32_t m_aniIndex;
		float m_aniFrame;
	};
}