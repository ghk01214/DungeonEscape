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

	struct TIMER_EVENT
	{
		std::chrono::steady_clock::time_point wakeUpTime;

		constexpr bool operator<(const TIMER_EVENT& left) const
		{
			return left.wakeUpTime < wakeUpTime;
		}
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

		void CreateThreads(std::thread& timer, std::thread& transform);
		void TimerThread();
		void TransformThread();
	public:
		void PushRecvMessage(Message& msg);
		void PushSendMessage(Message& msg);
		void ExecuteMessage();

		void PushEvent(TIMER_EVENT& ev);
		void PushTransformEvent(TIMER_EVENT& ev);
		void PushTransformMessage(Message& msg);

		void SetIOCPHandle(HANDLE iocp);

	private:
		void PopRecvQueue(std::queue<Message>& queue, tbb::concurrent_queue<Message>& concurrentQueue, int32_t size, std::atomic_int32_t& queueSiuze);
		int32_t NewObjectID();

		void Login(int32_t playerID, Player* player);
		void Logout(int32_t playerID, int32_t roomID, Player* player, ObjectManager* objMgr);

	private:
		HANDLE m_iocp;

		tbb::concurrent_queue<Message> m_recvQueue;
		tbb::concurrent_queue<Message> m_sendQueue;
		tbb::concurrent_queue<Message> m_transformMessage;

		std::atomic_int32_t m_recvQueueSize;
		std::atomic_int32_t m_sendQueueSize;

		tbb::concurrent_priority_queue<TIMER_EVENT> m_eventQueue;
		tbb::concurrent_priority_queue<TIMER_EVENT> m_transformEvent;

		std::atomic_int32_t m_objectsNum;
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableObjectID;
	};
}