#pragma once

#include "Room.h"

class UnitObject;

namespace game
{
	class CRoomManager
	{
		DeclareSingletone(CRoomManager);

	private:
		CRoomManager();
		~CRoomManager();
	public:
		void Init();
		void Release();

		int32_t CreateRoom();
		void DeleteRoom(int32_t roomID);

		void Enter(int32_t roomID, UnitObject* player);
		void Exit(int32_t roomID, UnitObject* player);

		const bool IsRoomCreated() const;

	private:
		int32_t NewRoomID();

	private:
		HashMap<int32_t, CRoom> m_rooms;
		std::atomic_int32_t m_roomSize;

		tbb::concurrent_priority_queue<int32_t> m_reusableRoomID;

		bool tempRoomCreated;
	};
}