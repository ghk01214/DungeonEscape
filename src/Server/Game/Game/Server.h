#pragma once

namespace network
{
	class OVERLAPPEDEX;
}

class GameInstance;

namespace game
{
	class CSession;

	class CServer
	{
	public:
		CServer();
		~CServer();

		void Run();
	private:
		CServer(const CServer& other) = delete;
		CServer& operator=(const CServer& other) = delete;

		void Initialize();
		void Accept();
		void CreateThread();

		void WorkerThread();
		void GameThread();
		void AcceptClient(network::OVERLAPPEDEX* pOverEx);
		void Recv(uint32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);
		void Send(uint32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);

		int32_t NewPlayerID();
		void Disconnect(uint32_t id);

#pragma region [PROCESS PACKET]
		void ProcessPacket(uint32_t id, network::CPacket& packet);
		void ProcessAUPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessMYPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessWRPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessBTPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessIFPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessITPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessCMPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessECPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessGMPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessTTPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol);
#pragma endregion

		void Login(uint32_t id, network::CPacket& packet);
	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_key;

		std::vector<std::thread> m_workerThreads;
		std::thread m_gameThreads;
		std::array<CSession*, MAX_USER> m_sessions;
		std::atomic_int32_t m_activeSessionNum;

		std::atomic_int32_t m_userID;
		std::uniform_int_distribution<int32_t> m_randomID;
		// 멀티스레드용 priority queue
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableID;

		// 멀티스레드용 map
		HashMap<uint16_t, class CObject*> m_objects;

		GameInstance* m_gameInstance;
	};
}