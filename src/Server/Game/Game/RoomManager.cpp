#include "pch.h"
#include "Player.h"
#include "RoomManager.h"

namespace game
{
	ImplementSingletone(CRoomManager);

	CRoomManager::CRoomManager()
	{
	}

	CRoomManager::~CRoomManager()
	{
	}

	void CRoomManager::Init()
	{
		m_roomSize = 0;
		tempRoomCreated = false;
	}

	void CRoomManager::Release()
	{
	}

	void CRoomManager::CreateRoom()
	{
		int32_t roomID{ NewRoomID() };

		CRoom newRoom;
		Accessor<int32_t, CRoom> access;

		bool flag{ false };

		while (flag == false)
		{
			flag = m_rooms.insert(access, roomID);

			if (flag == true)
			{
				newRoom.SetID(roomID);
				access->second = newRoom;
				tempRoomCreated = true;
			}
		}
	}

	void CRoomManager::DeleteRoom(int32_t roomID)
	{
		bool flag{ false };

		while (flag == false)
		{
			flag = m_rooms.erase(roomID);
		}
	}

	int32_t CRoomManager::NewRoomID()
	{
		bool issueNewID{ m_reusableRoomID.empty() };

		// 재사용 가능 id가 없으면 최고 숫자 발급
		if (issueNewID == true)
			return m_roomSize++;

		int32_t newID{ -1 };

		// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
		while (true)
		{
			bool issueReuseID{ m_reusableRoomID.try_pop(newID) };

			if (issueReuseID == true)
				return newID;
		}
	}

	void CRoomManager::Enter(int32_t roomID, Player* player)
	{
		Accessor<int32_t, CRoom> access;
		bool flag{ false };

		while (flag == false)
		{
			flag = m_rooms.find(access, roomID);
		}

		access->second.Enter(player);
	}

	void CRoomManager::Leave(int32_t roomID, Player* player)
	{
		Accessor<int32_t, CRoom> access;
		bool flag{ false };

		while (flag == false)
		{
			flag = m_rooms.find(access, roomID);
		}

		access->second.Leave(player);
	}

	const bool CRoomManager::IsRoomCreated() const
	{
		return tempRoomCreated;
	}

}