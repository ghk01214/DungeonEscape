#include "pch.h"
#include "Player.h"
#include "GameInstance.h"
#include "Room.h"
#include "TimeManager.h"
#include "MessageHandler.h"

namespace game
{
	CRoom::CRoom() :
		m_occupation{ 0 }
	{
	}

	CRoom::~CRoom()
	{
	}

	void CRoom::Enter(Player* player)
	{
		if (m_occupation >= 3)
		{
			std::cout << "room[" << m_id << "] fully occupied\n";
			return;
		}

		for (auto& [pl, filled] : m_players)
		{
			if (filled == true)
				continue;

			filled = true;
			pl = player;
			++m_occupation;
		}
	}

	void CRoom::Leave(Player* player)
	{
		for (auto& [pl, filled] : m_players)
		{
			if (filled == false)
				continue;

			if (pl == player)
			{
				pl == nullptr;
				filled = false;
			}
		}
	}

	void CRoom::StartGame()
	{
		// TODO : 게임 로직
	}

	void CRoom::GameLogicThread()
	{
		while (true)
		{
			double timeDelta{ TimeManager::GetInstance()->GetElapsedTime() };
			m_gameInstance->Update(timeDelta);
			m_gameInstance->LateUpdate(timeDelta);
			MessageHandler::GetInstance()->SendPacketMessage();
		}
	}

	constexpr int32_t CRoom::GetID() const
	{
		return m_id;
	}

	void CRoom::SetID(int32_t id)
	{
		m_id = id;
	}
}