#pragma once

#include <tbb/concurrent_priority_queue.h>
#include <tbb/concurrent_hash_map.h>
#include <Singleton.h>
#include <Packet.h>

namespace game
{
	SKIP_TCP_NAMESPACE;

	class CServer : public Singleton<CServer>
	{
	public:
		CServer(boost::asio::io_context* context, int32_t port);
		~CServer();

		void Run();

		void ProcessPacket(packet::CPacket& packet);
		void AuthorizePacketProcess(packet::CPacket& packet, uint32_t id, ProtocolID type);
		void MyPacketProcess(packet::CPacket& packet, uint32_t id, ProtocolID type);

		void DecreaseActiveSessionNum();
	private:
		void WorkerThread();

		void Accept();

		const uint32_t GetNewID();
		uint32_t GetNewAccessID();
	private:
		void SendMovePacket(uint32_t id);
		void SendRotatePacket(uint32_t id);

		void SendTargetNumPacket(uint32_t id);

	private:
		boost::asio::io_context* m_pContext;
		tcp::acceptor m_accpetor;
		tcp::socket m_socket;

		std::vector<std::thread> m_workerThreads;

		std::array<std::shared_ptr<class CSession>, MAX_USER> m_sessions;
		std::atomic_int32_t m_activeSessionNum;
		//tbb::concurrent_unordered_set<uint32_t> m_nonavailableID;

		std::atomic_int32_t m_userID;
		tbb::concurrent_priority_queue<uint32_t, std::greater<uint32_t>> m_reusableID;

		std::uniform_int_distribution<uint32_t> m_randomID;

		tbb::concurrent_hash_map<uint16_t, class CObject*> m_objects;
		uint16_t m_targetNum;
	};
}