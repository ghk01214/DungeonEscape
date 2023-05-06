#pragma once

//#include "UnitObject.h"
class UnitObject;

namespace game
{
	class CRoom
	{
	public:
		CRoom();
		~CRoom();

		void Enter(UnitObject* player);
		void Exit(UnitObject* player);

		void StartGame();
		void GameLogicThread();

		constexpr int32_t GetID() const;

		void SetID(int32_t id);

	private:
		int32_t m_id;

		std::array<std::pair<UnitObject*, bool>, 3> m_players;
		int32_t m_occupation;

		class GameInstance* m_gameInstance;
	};
}