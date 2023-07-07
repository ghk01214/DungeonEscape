﻿#include "pch.h"
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
#include "CapsuleCollider.h"
#include "Monster.h"

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

	void MessageHandler::Init(HANDLE iocp)
	{
		m_recvQueueSize = 0;
		m_sendQueueSize = 0;
		m_objectsNum = 0;
		m_iocp = iocp;
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
			postOver.objType = msg.objType;
			postOver.state = msg.state;

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
			postOver.objType = msg.objType;
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
		auto monsterObjects{ objMgr->GetLayer(L"Layer_Monster")->GetGameObjects() };

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

						if (player->GetID() == msg.playerID)
						{
							// 용섭 클라이언트 로그아웃
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
				}
				break;
				case ProtocolID::MY_ADD_ANIMATE_OBJ_REQ:
				{
					if (msg.objType == server::OBJECT_TYPE::PLAYER)
					{
						int32_t colliderID{ NewColliderID() };

						//Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(1500.f + msg.playerID * 50.f, 100.f, -1500.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };
						Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(0.f + msg.playerID * 50.f, 300.f, -2500.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };

						std::wstring name{};

						if (msg.fbxType == server::FBX_TYPE::NANA)
							name = L"Nana";
						else if (msg.fbxType == server::FBX_TYPE::MISTIC)
							name = L"Mistic";
						else if (msg.fbxType == server::FBX_TYPE::CARMEL)
							name = L"Carmel";

						player->SetName(name);
						player->SetObjectType(msg.objType);
						player->SetFBXType(msg.fbxType);
						player->GetController()->GetCollider()->SetID(colliderID);
					}

					Message sendMsg{ msg.playerID, ProtocolID::WR_ADD_ANIMATE_OBJ_ACK };
					sendMsg.objType = msg.objType;

					PushSendMessage(sendMsg);
				}
				break;
				case ProtocolID::MY_KEYINPUT_REQ:
				{
					for (auto& playerObject : playerObjects)
					{
						auto player{ dynamic_cast<Player*>(playerObject) };

						if (player->GetID() == msg.playerID)
						{
							player->GetController()->KeyboardReceive(msg.keyInput);
							break;
						}
					}
				}
				break;
				case ProtocolID::MY_ANI_PLAY_TIME_REQ:
				{
					if (msg.objType == server::OBJECT_TYPE::PLAYER)
					{
						for (auto& playerObj : playerObjects)
						{
							auto player{ dynamic_cast<Player*>(playerObj) };

							if (player->GetID() == msg.playerID)
							{
								player->SetAniPlayTime(msg.aniPlayTime);
								break;
							}
						}
					}
					else if (msg.objType == server::OBJECT_TYPE::BOSS)
					{
						for (auto& monsterObject : monsterObjects)
						{
							auto monster{ dynamic_cast<Monster*>(monsterObject) };

							if (monster->GetID() == msg.objID)
							{
								monster->SetAniPlayTime(msg.aniPlayTime);
								break;
							}
						}
					}
				}
				break;
				case ProtocolID::MY_ANI_END_REQ:
				{
					if (msg.objType == server::OBJECT_TYPE::PLAYER)
					{
						for (auto& playerObj : playerObjects)
						{
							auto player{ dynamic_cast<Player*>(playerObj) };

							if (player->GetID() == msg.objID)
							{
								player->SetAniEndFlag(true);
								break;
							}
						}
					}
					else if (msg.objType == server::OBJECT_TYPE::BOSS)
					{
						for (auto& monsterObject : monsterObjects)
						{
							auto monster{ dynamic_cast<Monster*>(monsterObject) };

							if (monster->GetID() == msg.objID)
							{
								monster->SetAniEndFlag(true);
								break;
							}
						}
					}
				}
				break;
				case ProtocolID::MY_CAMERA_LOOK_REQ:
				{
					for (auto& playerObj : playerObjects)
					{
						auto player{ dynamic_cast<Player*>(playerObj) };

						if (player->GetID() == msg.playerID)
						{
							player->SetControllerCameraLook(msg.cameraLook);
							break;
						}
					}
				}
				break;
#pragma endregion
				case ProtocolID::WR_REMOVE_REQ:
				{
					if (msg.objType == server::OBJECT_TYPE::BOSS)
					{
						for (auto& monsterObject : monsterObjects)
						{
							auto monster{ dynamic_cast<Monster*>(monsterObject) };

							if (monster->GetID() == msg.objID)
							{
								monster->SetRemoveReserved();
								break;
							}
						}
					}

					Message sendMsg{ -1, ProtocolID::WR_REMOVE_ACK };
					sendMsg.objID = msg.objID;
					sendMsg.objType = msg.objType;

					PushSendMessage(sendMsg);
				}
				break;
				default:
				break;
			}

			++i;
			queue.pop();
		}
	};

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

	void MessageHandler::PushTransformMessage(Message& msg)
	{
		m_transformMessage.push(msg);

		game::TIMER_EVENT ev{ CURRENT_TIME };
		m_transformEvent.push(ev);
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

	int32_t MessageHandler::NewColliderID()
	{
		if (m_reusableColliderID.empty() == true)
			return m_colliderNum++;

		int32_t newID{ m_reusableColliderID.top() };
		m_reusableColliderID.pop();

		return newID;
	}

	void MessageHandler::RemoveObject(int32_t objID)
	{
		m_reusableObjectID.push(objID);
		--m_objectsNum;
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