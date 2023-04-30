#pragma once

enum MessageType {LOGIN, LOGOUT};

struct Message
{
	MessageType	msgType;
	uint32_t	id = -1;

	Message(uint32_t _id, MessageType _msgType)
	{
		id = _id;
		msgType = _msgType;
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
public:
	void InsertMessage(uint32_t playerID, MessageType msgType);
	void ExecuteMessage();
private:
	std::queue<Message> m_messageQueue;
};


