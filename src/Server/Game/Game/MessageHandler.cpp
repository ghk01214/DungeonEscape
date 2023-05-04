#include "pch.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "MapObject.h"
#include "Player.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "RoomManager.h"

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
	m_recvQueueSize = 0;
	m_sendQueueSize = 0;
	m_sendBufferQueueSize = 0;
	m_objectsNum = 0;
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

void MessageHandler::SendPacketMessage()
{
	int32_t size{ m_sendQueueSize.load() };
	tbb::concurrent_queue<Message> queue{ m_sendQueue };

	if (size == 0)
		return;

	network::OVERLAPPEDEX overEX{ network::COMPLETION::BROADCAST };

	for (int32_t i = 0; i < size;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ queue.try_pop(msg) };

		if (success == false)
			continue;

		overEX.msgProtocol = msg.msgProtocol;
		overEX.targetID = msg.objID;

		if (msg.id == -1)
		{
			std::uniform_int_distribution<int32_t> uid{ 0, 2 };
			PostQueuedCompletionStatus(m_iocp, 1, uid(dre), &overEX.over);
		}
		else
			PostQueuedCompletionStatus(m_iocp, 1, msg.id, &overEX.over);

		++i;
	}

	PopSendQueue(size);
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

void MessageHandler::InsertSendMessage(Message msg)
{
	m_sendQueue.push(msg);
	++m_sendQueueSize;
}

void MessageHandler::ExecuteMessage()
{
	int32_t size{ m_recvQueueSize.load() };
	tbb::concurrent_queue<Message> queue{ m_recvQueue };

	if (size == 0)
		return;

	auto objMgr{ ObjectManager::GetInstance() };

	for (int32_t i = 0; i < size;)
	{
		Message msg{ -1, ProtocolID::PROTOCOL_NONE };
		bool success{ queue.try_pop(msg) };

		if (success == false)
			continue;

		switch (msg.msgProtocol)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				Player* playerObj{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.id, Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5)) };
				std::cout << msg.id << " : Create player\n";

				if (game::CRoomManager::GetInstance()->IsRoomCreated() == false)
					game::CRoomManager::GetInstance()->CreateRoom();

				game::CRoomManager::GetInstance()->Enter(0, playerObj);

				Message sendMsg{ msg.id, ProtocolID::AU_LOGIN_ACK };
				//m_sendQueue.push(sendMsg);
				InsertSendMessage(sendMsg);
			}
			break;
			case ProtocolID::MY_ADD_REQ:
			{
				int32_t objID{ NewObjectID() };

				// 오브젝트 추가 작업 후 id 세팅
				auto MapPlaneObject = objMgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 2, 0), Quat(0, 0, 0, 1), Vec3(100, 2, 100));
				MapPlaneObject->SetID(objID);
				auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
				MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());

				Message sendMsg{ -1, ProtocolID::WR_ADD_ACK };
				sendMsg.objID = objID;

				//m_sendQueue.push(sendMsg);
				InsertSendMessage(sendMsg);
			}
			break;
		}

		++i;
	}

	PopRecvQueue(size);
}

void MessageHandler::SetIOCPHandle(HANDLE iocp)
{
	m_iocp = iocp;
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

int32_t MessageHandler::NewObjectID()
{
	bool issueNewID{ m_reusableObjectID.empty() };

	// 재사용 가능 id가 없으면 최고 숫자 발급
	if (issueNewID == true)
		return (m_objectsNum++) + 3;

	int32_t newID{ -1 };

	// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
	while (true)
	{
		bool issueReuseID{ m_reusableObjectID.try_pop(newID) };

		if (issueReuseID == true)
			return newID;
	}

	return 0;
}
