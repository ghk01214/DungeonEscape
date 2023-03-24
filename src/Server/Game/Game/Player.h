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

		void SetAniIndex(int32_t index) { m_aniIndex = index; }

	private:
		int32_t m_aniIndex;
	};
}