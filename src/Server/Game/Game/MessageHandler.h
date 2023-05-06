#pragma once

class ObjectManager;
class UnitObject;

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

		void SetIOCPHandle(HANDLE iocp);

	private:
		void PopRecvQueue(int32_t size);
		void PopSendQueue(int32_t size);
		int32_t NewObjectID();

		void Login(int32_t playerID, UnitObject* player);
		void Logout(int32_t playerID, int32_t roomID, UnitObject* player, ObjectManager* objMgr);

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