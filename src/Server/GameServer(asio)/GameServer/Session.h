#pragma once

//#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_priority_queue.h>
#include <Packet.h>
#include "Player.h"

namespace game
{
	SKIP_TCP_NAMESPACE;

	class CServer;

	class CSession :
		public std::enable_shared_from_this<CSession>
	{
	public:

		CSession(CServer* server, tcp::socket socket);
		~CSession();

		void Run(uint32_t id);

		void Recv();
		void Send(packet::CPacket& packet);

		void Move(packet::CPacket& packet);
		void Rotate(packet::CPacket& packet);

		const CObject* GetPlayer() const { return m_player; }

	private:
		CServer* m_server;
		tcp::socket m_socket;

		std::array<uint8_t, packet::CPacket::BUFF_SIZE> m_packet;
		packet::CPacket m_recvPacket;
		uint16_t m_recvPacketSize;
		uint16_t m_prevPacketSize;

		CObject* m_player;
	};
}
