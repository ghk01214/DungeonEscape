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
#include "TimeManager.h"

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

	void MessageHandler::CreateThreads(std::thread& timer, std::thread& transform)
	{
		timer = std::thread{ &MessageHandler::TimerThread, this };
		transform = std::thread{ &MessageHandler::TransformThread, this };
	}

	void MessageHandler::TimerThread()
	{
		using namespace std::chrono_literals;
		while (true)
		{
			TIMER_EVENT ev;
			auto currentTime{ std::chrono::steady_clock::now() };
			bool success{ m_eventQueue.try_pop(ev) };

			if (success == false)
				continue;

			if (ev.wakeUpTime > currentTime)
			{
				m_eventQueue.push(ev);
				continue;
			}

			Message msg{};
			while (true)
			{
				bool success{ m_sendQueue.try_pop(msg) };

				if (success == true)
				{
					--m_sendQueueSize;
					break;
				}
			}

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			network::CPacket packet;
			postOver.msgProtocol = msg.msgProtocol;
			postOver.playerID = msg.playerID;
			postOver.objID = msg.objID;
			postOver.roomID = msg.roomID;

			PostQueuedCompletionStatus(m_iocp, 1, msg.playerID, &postOver.over);

			std::this_thread::sleep_for(1ms);
		}
	}

	void MessageHandler::TransformThread()
	{
		using namespace std::chrono_literals;

		while (true)
		{
			TIMER_EVENT ev;
			auto currentTime{ std::chrono::steady_clock::now() };
			bool success{ m_transformEvent.try_pop(ev) };

			if (success == false)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}

			if (ev.wakeUpTime > currentTime)
			{
				m_transformEvent.push(ev);
				std::this_thread::sleep_for(1ms);
				continue;
			}

			Message msg{};
			while (true)
			{
				bool success{ m_transformMessage.try_pop(msg) };

				if (success == true)
					break;
			}

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			postOver.msgProtocol = msg.msgProtocol;
			postOver.playerID = msg.playerID;
			postOver.objID = msg.objID;
			postOver.roomID = msg.roomID;

			PostQueuedCompletionStatus(m_iocp, 1, msg.playerID, &postOver.over);

			std::this_thread::sleep_for(1ms);
		}
	}

	void MessageHandler::ExecuteMessage()
	{
		int32_t size{ m_recvQueueSize.load() };

		if (size == 0)
			return;

		std::queue<Message> queue;

		for (int32_t i = 0; i < size;)
		{
			bool empty{ m_recvQueue.empty() };

			if (empty == true)
				break;

			Message msg{};
			bool success{ m_recvQueue.try_pop(msg) };

			if (success == true)
			{
				queue.push(msg);
				--m_recvQueueSize;
				++i;
			}
		}

		auto objMgr{ ObjectManager::GetInstance() };
		auto playerObjects{ objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (int32_t i = 0; i < size;)
		{
			Message msg{ queue.front() };

			switch (msg.msgProtocol)
			{
#pragma region [AU]
				case ProtocolID::AU_LOGIN_REQ:
				{
					//Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(msg.playerID * 20.f, 0.f, 0.f), Quat(0, 0, 0, 1), Vec3(1.f, 1.f, 1.f)) };
					//player->SetName(L"Mistic");
					//
					//Login(msg.playerID, player);
					Message sendMsg{ msg.playerID, ProtocolID::AU_LOGIN_ACK };
					PushSendMessage(sendMsg);
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
#pragma endregion
#pragma region [MY]
				case ProtocolID::MY_ISSUE_PLAYER_ID_REQ:
				case ProtocolID::WR_ISSUE_PLAYER_ID_REQ:
				{
					Message sendMsg{ msg.playerID, static_cast<ProtocolID>(magic_enum::enum_integer(msg.msgProtocol) + 1) };
					PushSendMessage(sendMsg);

					TIMER_EVENT ev{ CURRENT_TIME };
					m_eventQueue.push(ev);
				}
				break;
				case ProtocolID::MY_ADD_ANIMATE_OBJ_REQ:
				{
					Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(msg.playerID * 100.f, 150.f, msg.playerID * 50.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };					player->SetName(L"Mistic");

					//Login(msg.playerID, player);

					Message sendMsg{ msg.playerID, ProtocolID::WR_ADD_ANIMATE_OBJ_ACK };
					PushSendMessage(sendMsg);
				}
				break;
				case ProtocolID::MY_ADD_OBJ_REQ:
				{
					int32_t objID{ NewObjectID() };

					// 오브젝트 추가 작업 후 id 세팅
					//auto MapPlaneObject = objMgr->AddGameObjectToLayer<MapObject>(L"Layer_Map", Vec3(0, 2, 0), Quat(0, 0, 0, 1), Vec3(100, 2, 100));
					//MapPlaneObject->SetID(objID);
					//auto MapPlaneBody = MapPlaneObject->GetComponent<RigidBody>(L"RigidBody");
					//MapPlaneBody->AddCollider<BoxCollider>(MapPlaneObject->GetTransform()->GetScale());

					Message msg{ -1, ProtocolID::WR_ADD_OBJ_ACK };
					msg.objID = objID;

					PushSendMessage(msg);
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
							player->ClientRequestStateChange(msg.aniIndex, msg.aniFrame);
							//player->SetAniIndex(msg.aniIndex);
							//player->SetAniFrame(msg.aniFrame);
							break;
						}
					}

					Message sendMsg{ msg.playerID, ProtocolID::WR_ANI_ACK };
					PushSendMessage(sendMsg);
				}
				break;
				case ProtocolID::MY_CAMERA_LOOK_REQ:
				{
					for (auto& playerObj : playerObjects)
					{
						auto player{ dynamic_cast<Player*>(playerObj) };

						if (player->GetPlayerID() == msg.playerID)
						{
							std::cout << msg.playerID << " : " << msg.cameraLook.x << ", " << msg.cameraLook.y << ", " << msg.cameraLook.z << "\n";
							// 플레이어 카메라 정보
							//msg.cameraLook;
							break;
						}
					}
				}
				break;
#pragma endregion
				default:
				break;
			}

			++i;
			queue.pop();
		}
	}

	void MessageHandler::PushRecvMessage(Message& msg)
	{
		m_recvQueue.push(msg);
		++m_recvQueueSize;
	}

	void MessageHandler::PushSendMessage(Message& msg)
	{
		m_sendQueue.push(msg);
		++m_sendQueueSize;

		TIMER_EVENT ev{ CURRENT_TIME };
		m_eventQueue.push(ev);
	}

	void MessageHandler::PushEvent(TIMER_EVENT& ev)
	{
		m_eventQueue.push(ev);
	}

	void MessageHandler::PushTransformEvent(TIMER_EVENT& ev)
	{
		m_transformEvent.push(ev);
	}

	void MessageHandler::PushTransformMessage(Message& msg)
	{
		m_transformMessage.push(msg);

		game::TIMER_EVENT ev{ CURRENT_TIME };
		m_transformEvent.push(ev);
	}

	void MessageHandler::SetIOCPHandle(HANDLE iocp)
	{
		m_iocp = iocp;
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

		PushSendMessage(msg);
	}

	void MessageHandler::Logout(int32_t playerID, int32_t roomID, Player* player, ObjectManager* objMgr)
	{
		//game::CRoomManager::GetInstance()->Exit(roomID, player);
		objMgr->RemoveGameObjectFromLayer(L"Layer_Player", player);

		Message msg{ playerID, ProtocolID::AU_LOGOUT_ACK };
		msg.roomID = roomID;

		PushSendMessage(msg);
	}
}