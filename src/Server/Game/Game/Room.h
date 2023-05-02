#pragma once

//#include "Player.h"
class Player;

namespace game
{
	class CRoom
	{
	public:
		CRoom();
		~CRoom();

		void Enter(Player* player);
		void Leave(Player* player);

		void StartGame();
		void GameLogicThread();

		constexpr int32_t GetID() const;

		void SetID(int32_t id);

	private:
		int32_t m_id;

		std::array<std::pair<Player*, bool>, 3> m_players;
		int32_t m_occupation;

		class GameInstance* m_gameInstance;
	};
}