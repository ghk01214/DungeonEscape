#pragma once

class ObjectManager;
class Player;

namespace game
{
	struct Message
	{
		ProtocolID	msgProtocol;
		int32_t		playerID;
		int32_t		objID;
		int32_t		roomID;
		ulong32_t	keyInput;
		int32_t		aniIndex;
		float		aniFrame;

		Message(int32_t id = -1, ProtocolID msgProtocol = ProtocolID::PROTOCOL_NONE);
	};

	class MessageHandler
	{
	public:
		DeclareSingletone(MessageHandler);

	private:
		MessageHandler();
		~MessageHandler();
	public:
		void Init();
		void Release();
	public:
		void InsertRecvMessage(Message msg);
		void InsertSendMessage(Message msg);
		void ExecuteMessage();
		void SendPacketMessage();

		Message PopMessage();

		void SetIOCPHandle(HANDLE iocp);

	private:
		void PopConcurrentQueue(std::queue<Message>& queue, tbb::concurrent_queue<Message>& concurrentQueue, int32_t size, std::atomic_int32_t& queueSiuze);
		int32_t NewObjectID();

		void Login(int32_t playerID, Player* player);
		void Logout(int32_t playerID, int32_t roomID, Player* player, ObjectManager* objMgr);

	private:
		HANDLE m_iocp;

		tbb::concurrent_queue<Message> m_recvQueue;
		tbb::concurrent_queue<Message> m_sendQueue;

		std::atomic_int32_t m_recvQueueSize;
		std::atomic_int32_t m_sendQueueSize;

		std::atomic_int32_t m_objectsNum;
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableObjectID;
	};
}