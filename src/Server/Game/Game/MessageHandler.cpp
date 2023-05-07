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
#include "CustomController.h"

namespace game
{
	ImplementSingletone(MessageHandler);

	Message::Message(int32_t id, ProtocolID msgProtocol) :
		playerID{ id },
		msgProtocol{ msgProtocol }
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
		m_objectsNum = 0;
	}

	void MessageHandler::Release()
	{
		bool empty{ m_recvQueue.empty() };

		while (empty == false)
		{
			Message msg{};
			m_recvQueue.try_pop(msg);
			empty = m_recvQueue.empty();
		}

		empty = m_sendQueue.empty();

		while (empty == false)
		{
			Message msg{};
			m_sendQueue.try_pop(msg);
			empty = m_sendQueue.empty();
		}
	}

	void MessageHandler::SendPacketMessage()
	{
		int32_t size{ m_sendQueueSize.load() };

		if (size == 0)
			return;

		std::queue<Message> queue;
		PopConcurrentQueue(queue, m_sendQueue, size, m_sendQueueSize);

		for (int32_t i = 0; i < size;)
		{
			Message msg{ queue.front() };

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			postOver.msgProtocol = msg.msgProtocol;
			postOver.playerID = msg.playerID;
			postOver.objID = msg.objID;
			postOver.roomID = msg.roomID;

			if (msg.playerID == -1)
			{
				std::uniform_int_distribution<int32_t> uid{ 0, 2 };
				PostQueuedCompletionStatus(m_iocp, 1, uid(dre), &postOver.over);
			}
			else
				PostQueuedCompletionStatus(m_iocp, 1, msg.playerID, &postOver.over);

			++i;
			queue.pop();
		}
	}

	void MessageHandler::InsertRecvMessage(Message msg)
	{
		m_recvQueue.push(msg);
		++m_recvQueueSize;
	}

	void MessageHandler::InsertSendMessage(Message msg)
	{
		m_sendQueue.push(msg);
		++m_sendQueueSize;
	}

	void MessageHandler::ExecuteMessage()
	{
		int32_t size{ m_recvQueueSize.load() };

		if (size == 0)
			return;

		std::queue<Message> queue;
		PopConcurrentQueue(queue, m_recvQueue, size, m_recvQueueSize);

		auto objMgr{ ObjectManager::GetInstance() };
		auto playerObjects{ objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (int32_t i = 0; i < size;)
		{
			Message msg{ queue.front() };

			switch (msg.msgProtocol)
			{
				case ProtocolID::AU_LOGIN_REQ:
				{
					Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(msg.playerID * 2.f, 10.f, -5.f), Quat(0, 0, 0, 1), Vec3(0.3f, 0.3f, 0.3f)) };
					player->SetName(L"Mistic");

					Login(msg.playerID, player);
				}
				break;
				case ProtocolID::AU_LOGOUT_REQ:
				{
					Player* player{ nullptr };

					for (auto& playerObj : playerObjects)
					{
						player = dynamic_cast<Player*>(playerObj);

						if (player->GetPlayerID() == msg.playerID)
						{
							Logout(msg.playerID, msg.roomID, player, objMgr);
							break;
						}
					}
				}
				break;
				case ProtocolID::MY_ADD_OBJ_REQ:
				{
					int32_t objID{ NewObjectID() };

					// 오브젝트 추가 작업 후 id 세팅
					auto MapPlaneObject = objMgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 2, 0), Quat(0, 0, 0, 1), Vec3(100, 2, 100));
					MapPlaneObject->SetID(objID);
					auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
					MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());

					Message msg{ -1, ProtocolID::WR_ADD_OBJ_ACK };
					msg.objID = objID;

					InsertSendMessage(msg);
				}
				break;
				case ProtocolID::MY_KEYINPUT_REQ:
				{
					auto playerLayer{ ObjectManager::GetInstance()->GetLayer(L"Layer_Player") };

					for (auto& playerObject : playerLayer->GetGameObjects())
					{
						auto player{ dynamic_cast<Player*>(playerObject) };

						if (player->GetPlayerID() == msg.playerID)
						{
							auto customController{ player->GetComponent<CustomController>(L"CustomController") };
							customController->KeyboardReceive(msg.keyInput);
							break;
						}
					}
				}
				break;
				case ProtocolID::MY_ANI_REQ:
				{
					for (auto& playerObj : playerObjects)
					{
						auto player{ dynamic_cast<Player*>(playerObj) };

						if (player->GetPlayerID() == msg.playerID)
						{
							player->SetAniIndex(msg.aniIndex);
							player->SetAniFrame(msg.aniFrame);
							break;
						}
					}

					Message sendMsg{ msg.playerID, ProtocolID::MY_ANI_ACK };
					InsertSendMessage(sendMsg);
				}
				break;
				default:
				break;
			}

			++i;
			queue.pop();
		}
	}

	void MessageHandler::SetIOCPHandle(HANDLE iocp)
	{
		m_iocp = iocp;
	}

	void MessageHandler::PopConcurrentQueue(std::queue<Message>& queue, tbb::concurrent_queue<Message>& concurrentQueue, int32_t size, std::atomic_int32_t& queueSize)
	{
		for (int32_t i = 0; i < size;)
		{
			Message msg{};
			bool success{ concurrentQueue.try_pop(msg) };

			if (success == true)
			{
				queue.push(msg);
				--queueSize;
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

	void MessageHandler::Login(int32_t playerID, Player* player)
	{
		int32_t roomID{};
		if (game::CRoomManager::GetInstance()->IsRoomCreated() == false)
			roomID = game::CRoomManager::GetInstance()->CreateRoom();

		game::CRoomManager::GetInstance()->Enter(roomID, player);

		Message msg{ playerID, ProtocolID::AU_LOGIN_ACK };
		msg.roomID = roomID;

		InsertSendMessage(msg);
	}

	void MessageHandler::Logout(int32_t playerID, int32_t roomID, Player* player, ObjectManager* objMgr)
	{
		game::CRoomManager::GetInstance()->Exit(roomID, player);
		objMgr->RemoveGameObjectFromLayer(L"Layer_Player", player);

		Message msg{ playerID, ProtocolID::AU_LOGOUT_ACK };
		msg.roomID = roomID;

		InsertSendMessage(msg);
	}
}