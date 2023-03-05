#pragma once

namespace network
{
	//enum class COMPLETION;
	class OVERLAPPEDEX;
}

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
		void AcceptClient(network::OVERLAPPEDEX* pOverEx);
		void Recv(ULONG_PTR id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);
		void Send(ULONG_PTR id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);

		int NewPlayerID();
		void Disconnect(ULONG_PTR id);

		void ProcessPacket(ULONG_PTR id, network::CPacket& packet);
		void ProcessAUPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);
		void ProcessMYPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);
		void ProcessBTPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);
		void ProcessITPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);
		void ProcessCMPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);
		void ProcessTTPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type);

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_key;

		std::vector<std::thread> m_workerThreads;
		std::array<CSession*, MAX_USER> m_sessions;
		std::atomic_int32_t m_activeSessionNum;

		std::atomic_int32_t m_userID;
		std::uniform_int_distribution<int32_t> m_randomID;
		// 멀티스레드용 priority queue
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableID;

		// 멀티스레드용 map
		tbb::concurrent_hash_map<uint16_t, class CObject*> m_objects;
	};
}