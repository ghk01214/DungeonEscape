#pragma once

class GameObject;
class ObjectManager;
class Player;

namespace game
{
	struct Message
	{
		ProtocolID				msgProtocol;
		int32_t					playerID;
		int32_t					objID;
		server::OBJECT_TYPE		objType;
		server::FBX_TYPE		fbxType;
		int32_t					roomID;
		ulong32_t				keyInput;
		float					aniPlayTime;
		Quat					playerLook;
		Vec3					cameraLook;
		int32_t					state;

		Message(int32_t playerID = -1, ProtocolID msgProtocol = ProtocolID::PROTOCOL_NONE);
	};

	struct TIMER_EVENT
	{
		std::chrono::steady_clock::time_point	wakeUpTime;
		ProtocolID								type;
		int32_t									playerID;
		int32_t									objID;
		int32_t									roomID;
		server::OBJECT_TYPE						objType;
		int32_t									state;
		Vec3									effectPos;
		int32_t									integer;

		TIMER_EVENT(ProtocolID msgProtocol = ProtocolID::PROTOCOL_NONE, int32_t playerID = -1);

		constexpr bool operator<(const TIMER_EVENT& left) const;
	};

	class MessageHandler
	{
	public:
		DeclareSingletone(MessageHandler);

	private:
		MessageHandler();
		~MessageHandler();
	public:
		void Init(HANDLE iocp);
		void Release();

		void TimerThread();
		void TransformThread();
	public:
		void PushRecvMessage(Message& msg);
		void PushSendMessage(TIMER_EVENT& ev);
		void ExecuteMessage();

		void PushTransformMessage(TIMER_EVENT& ev);

	public:
		int32_t NewObjectID();

		void RemoveObject();

		int32_t GetObjectNum() { return m_objectsNum; }

	private:
		void Login(int32_t playerID, Player* player);
		void Logout(int32_t playerID, int32_t roomID, Player* player);
		void CreatePlayer(Message& msg);
		void SetKeyInput(Message& msg);
		void SetAniEndFlag(Message& msg);
		void SetPlayerCameraLook(Message& msg);
		void SetPlayerLook(Message& msg);

	private:
		HANDLE m_iocp;

		tbb::concurrent_queue<Message> m_recvQueue;
		tbb::concurrent_priority_queue<TIMER_EVENT> m_sendQueue;
		tbb::concurrent_priority_queue<TIMER_EVENT> m_transformQueue;

		std::atomic_int32_t m_recvQueueSize;

		std::atomic_int32_t m_objectsNum;
		std::atomic_int32_t m_objectIDOffest;
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableObjectID;

		ObjectManager* m_objMgr;
	};
}