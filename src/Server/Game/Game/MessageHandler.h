#pragma once

struct Message
{
	ProtocolID	msgProtocol;
	int32_t		id;

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

	tbb::concurrent_queue<Message> GetSendQueue(int32_t& size);

private:
	void PopRecvQueue(int32_t size);
	void PopSendQueue(int32_t size);

private:
	tbb::concurrent_queue<Message> m_recvQueue;
	tbb::concurrent_queue<Message> m_sendQueue;
	std::atomic_int32_t m_recvQueueSize;
	std::atomic_int32_t m_sendQueueSize;
};


