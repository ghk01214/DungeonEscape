#pragma once
#include <tbb/concurrent_hash_map.h>

namespace chat
{
	class CRoom
	{
	private:
		using PlayerMap = tbb::concurrent_hash_map<uint16_t, CPlayer>;
	public:
		CRoom();
		~CRoom();

		void Join(const uint16_t id);
		void Leave(const uint16_t id);
		void Broadcast();

	private:
		PlayerMap m_players;
		tbb::concurrent_queue<packet::CPacket> m_recentMessages;
	};
}