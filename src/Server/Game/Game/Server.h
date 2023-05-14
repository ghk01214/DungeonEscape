#pragma once

namespace network
{
	class OVERLAPPEDEX;
}

class GameInstance;
class GameObject;
class Player;

namespace game
{
	class CSession;
	class CRoomManager;
	struct Message;
	struct TIMER_EVENT;

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
		void Recv(int32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);
		void Send(int32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx);

		int32_t NewPlayerID();
		void Disconnect(int32_t id);

#pragma region [PROCESS PACKET]
		void ProcessPacket(int32_t id, network::CPacket& packet);
		void ProcessAUPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessMYPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessWRPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessBTPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessIFPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessITPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessCMPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessECPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessGMPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
		void ProcessTTPacket(int32_t id, network::CPacket& packet, ProtocolID protocol);
#pragma endregion

		void Login(int32_t id, CSession* session, Player* player, int32_t roomID);
		void Logout(int32_t id);
		void BroadcastResult(int32_t id, network::OVERLAPPEDEX* over);
		void InputCommandMessage(Message msg);
		Player* FindPlayer(std::list<GameObject*>& playerObjects, int32_t playerID);

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_key;

		std::vector<std::thread> m_workerThreads;
		std::thread m_gameThread;
		std::thread m_timerThread;
		std::thread m_transformThread;

		std::array<CSession*, MAX_USER> m_sessions;
		std::atomic_int32_t m_activeSessionNum;

		std::atomic_int32_t m_userID;
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableID;

		tbb::concurrent_priority_queue<TIMER_EVENT> m_eventQueue;

		CRoomManager* m_roomManager;
		GameInstance* m_gameInstance;
	};
}