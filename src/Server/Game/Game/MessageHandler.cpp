#include "pch.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"

Message::Message(int32_t _id, ProtocolID _msgProtocol) :
	id{ _id },
	msgProtocol{ _msgProtocol }
{
}

ImplementSingletone(MessageHandler);

MessageHandler::MessageHandler()
{
}

MessageHandler::~MessageHandler()
{
	Release();
}

void MessageHandler::Init()
{
}

void MessageHandler::Release()
{
	bool empty{ m_recvQueue.empty() };

	while (empty == false)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		m_recvQueue.try_pop(msg);
		empty = m_recvQueue.empty();
	}
}

void MessageHandler::InsertRecvMessage(int32_t playerID, ProtocolID msgProtocol)
{
	Message msg{ playerID, msgProtocol };
	m_recvQueue.push(msg);
}

void MessageHandler::InsertSendMessage(int32_t playerID, ProtocolID msgProtocol)
{
	Message msg{ playerID, msgProtocol };
	m_sendQueue.push(msg);
	++m_sendQueueSize;
}

void MessageHandler::ExecuteMessage()
{
	bool empty{ m_recvQueue.empty() };

	if (empty == true)
		return;

	tbb::concurrent_queue<Message> queue{ m_recvQueue };
	int32_t size{ m_recvQueueSize.load() };

	if (queue.empty() == false)
		PopRecvQueue(size);

	auto objMgr{ ObjectManager::GetInstance() };

	while (queue.empty() == false)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ queue.try_pop(msg) };

		if (success == false)
			continue;

		switch (msg.msgProtocol)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.id, Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5));
				//피직스 시뮬레이션에서 오브젝트를 추가하는거랑 sesssion의 명령 두개가 아주 약간의 시간차는 둬도 되지않나?
				//상관없으면 저 두 라인은 server.cpp에 그대로 냅두는것도 괜찮을듯. 일단 냄겨둠
			}
			break;
		}
	}
}

void MessageHandler::PopRecvQueue(int32_t size)
{
	for (int32_t i = 0; i < size;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ m_recvQueue.try_pop(msg) };

		if (success == true)
		{
			--m_recvQueueSize;
			++i;
		}
	}
}

void MessageHandler::PopSendQueue(int32_t size)
{
	for (int32_t i = 0; i < size;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ m_sendQueue.try_pop(msg) };

		if (success == true)
		{
			--m_sendQueueSize;
			++i;
		}
	}
}

tbb::concurrent_queue<Message> MessageHandler::GetSendQueue(int32_t& size)
{
	auto queue{ m_sendQueue };
	size = m_sendQueueSize.load();

	if (queue.empty() == false)
		PopSendQueue(size);

	return queue;
}
