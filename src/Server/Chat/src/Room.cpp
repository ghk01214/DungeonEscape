#include <tbb/concurrent_queue.h>
#include <Packet.h>
#include "Player.h"
#include "Room.h"

namespace chat
{
	CRoom::CRoom()
	{
	}

	CRoom::~CRoom()
	{
	}

	void CRoom::Join(const uint16_t id)
	{
		CPlayer newPlayer{ id };

		PlayerMap::accessor writer;
		m_players.insert(writer, id);
		writer->second = newPlayer;
	}

	void CRoom::Leave(const uint16_t id)
	{
		PlayerMap::accessor writer;
		m_players.erase(id);
	}

	void CRoom::Broadcast()
	{
	}
}
