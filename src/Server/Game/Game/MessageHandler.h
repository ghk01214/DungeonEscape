#pragma once

struct Message
{
	ProtocolID	msgProtocol;
	int32_t		id;
	int32_t		objID;

	Message(int32_t _id = -1, ProtocolID _msgProtocol = ProtocolID::PROTOCOL_NONE);
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
	void InsertRecvMessage(int32_t playerID, ProtocolID msgProtocol);
	void InsertSendMessage(int32_t playerID, ProtocolID msgProtocol);
	void ExecuteMessage();
	void SendPacketMessage();

	void SetIOCPHandle(HANDLE iocp);

private:
	void CopySendQueue();
	void PopRecvQueue(int32_t size);
	void PopSendQueue(int32_t size);
	int32_t NewObjectID();

private:
	HANDLE m_iocp;

	tbb::concurrent_queue<Message> m_recvQueue;
	tbb::concurrent_queue<Message> m_sendQueue;
	tbb::concurrent_queue<Message> m_sendBufferQueue;

	std::atomic_int32_t m_recvQueueSize;
	std::atomic_int32_t m_sendQueueSize;
	std::atomic_int32_t m_sendBufferQueueSize;

	std::atomic_int32_t m_objectsNum;
	tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableObjectID;
};


