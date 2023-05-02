#include "pch.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"

ImplementSingletone(MessageHandler);

Message::Message(int32_t _id, ProtocolID _msgProtocol) :
	id{ _id },
	msgProtocol{ _msgProtocol }
{
}

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

	empty = m_sendQueue.empty();

	while (empty == false)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		m_sendQueue.try_pop(msg);
		empty = m_sendQueue.empty();
	}

	empty = m_sendBufferQueue.empty();

	while (empty == false)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		m_sendBufferQueue.try_pop(msg);
		empty = m_sendBufferQueue.empty();
	}
}

void MessageHandler::CopySendQueue()
{
	int32_t size{ m_sendQueueSize.load() };
	m_sendBufferQueueSize = size;

	for (int32_t i = 0; i < size;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ m_sendQueue.try_pop(msg) };

		if (success == false)
			continue;

		m_sendBufferQueue.push(msg);
		--m_sendQueueSize;
		++i;
	}
}

void MessageHandler::SendPacketMessage(HANDLE iocp, network::OVERLAPPEDEX& over)
{
	if (m_sendBufferQueue.empty() == true)
		return;

	for (int32_t i = 0; i < m_sendBufferQueueSize;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ m_sendBufferQueue.try_pop(msg) };

		if (success == false)
			continue;

		over.msgProtocol = msg.msgProtocol;
		PostQueuedCompletionStatus(iocp, 1, msg.id, &over.over);
		++i;
	}
}

void MessageHandler::InsertRecvMessage(int32_t playerID, ProtocolID msgProtocol)
{
	Message msg{ playerID, msgProtocol };
	m_recvQueue.push(msg);
	++m_recvQueueSize;
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
				std::cout << std::format("{} : Create player\n", msg.id);

				Message sendMsg{ msg.id, ProtocolID::AU_LOGIN_ACK };
				m_sendQueue.push(sendMsg);
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
