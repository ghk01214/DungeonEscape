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
	};
}