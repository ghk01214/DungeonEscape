#pragma once
#include <Packet.h>
#include <tbb/concurrent_priority_queue.h>

namespace chat
{
	SKIP_TCP_NAMESPACE;
	SKIP_STEADY_CLOCK_NAMESPACE;

	class CServer : public CPlayer, public std::enable_shared_from_this<CServer>
	{
	private:
		enum class STATE
		{
			NONE = 0,
			ACCEPTED,
			ONLINE,
			MAX
		};
	private:
		using PacketData = std::array<uint8_t, packet::CPacket::BUFF_SIZE>;
	public:
		CServer(tcp::socket socket);
		~CServer();

		void Run();
	private:
		void InitPlayers();

		void Send(packet::CPacket& packet, uint32_t id);
		void ProcessPacket(packet::CPacket& packet, int32_t id);

		void Read();
		void Write(packet::CPacket& packet);

		void EmptyPacket(PacketData& packet);

		const int32_t GetNewID();

	private:
		tcp::socket m_socket;
		PacketData m_recvData;
		PacketData m_sendData;
		packet::CPacket m_packet;
		int16_t m_recvPacketSize;
		int32_t m_prevDataSize;
		int32_t m_id;

		static std::array<CPlayer, MAX_USER> m_players;
		static std::atomic_int32_t m_playerNum;
		static std::atomic_int32_t m_userID;
		static tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reuseID;
	};
}