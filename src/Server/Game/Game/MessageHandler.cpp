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
#include "CapsuleCollider.h"
#include "Monster.h"
#include "Weeper.h"

namespace game
{
	ImplementSingletone(MessageHandler);

	Message::Message(int32_t playerID, ProtocolID msgProtocol) :
		playerID{ playerID },
		msgProtocol{ msgProtocol }
	{
	}

	TIMER_EVENT::TIMER_EVENT(ProtocolID msgProtocol, int32_t playerID) :
		playerID{ playerID },
		type{ msgProtocol },
		wakeUpTime{ CURRENT_TIME }
	{
	}

	constexpr bool TIMER_EVENT::operator<(const TIMER_EVENT& left) const
	{
		return left.wakeUpTime < wakeUpTime;
	}

	//===============================================================================

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
		m_objectsNum = 0;
		m_objectIDOffest = 100;
		m_iocp = iocp;

		m_objMgr = ObjectManager::GetInstance();
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
			TIMER_EVENT ev{};
			m_sendQueue.try_pop(ev);
			empty = m_sendQueue.empty();
		}
	}

	void MessageHandler::TimerThread()
	{
		using namespace std::chrono_literals;

		while (true)
		{
			TIMER_EVENT ev;
			auto currentTime{ CURRENT_TIME };
			//bool success{ m_sendQueue.try_pop(ev) };

			if (m_sendQueue.try_pop(ev) == false)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}

			if (ev.wakeUpTime > currentTime)
			{
				m_sendQueue.push(ev);
				std::this_thread::sleep_for(1ms);
				continue;
			}

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			postOver.msgProtocol = ev.type;
			//postOver.roomID = ev.roomID;

			switch (ev.type)
			{
				case ProtocolID::AU_LOGIN_ACK:
				case ProtocolID::AU_LOGOUT_ACK:
				case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
				{
					PostQueuedCompletionStatus(m_iocp, 1, ev.playerID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
				case ProtocolID::WR_ADD_OBJ_ACK:
				case ProtocolID::WR_REMOVE_ACK:
				{
					postOver.objType = ev.objType;

					PostQueuedCompletionStatus(m_iocp, 1, ev.objID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
				case ProtocolID::WR_CHANGE_STATE_ACK:
				{
					postOver.objType = ev.objType;
					postOver.state = ev.state;

					PostQueuedCompletionStatus(m_iocp, 1, ev.playerID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
				default:
				std::this_thread::sleep_for(1ms);
				break;
			}
		}
	}

	void MessageHandler::TransformThread()
	{
		using namespace std::chrono_literals;

		while (true)
		{
			TIMER_EVENT ev;
			auto currentTime{ CURRENT_TIME };
			bool success{ m_transformQueue.try_pop(ev) };

			if (success == false)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}

			if (ev.wakeUpTime > currentTime)
			{
				m_transformQueue.push(ev);
				std::this_thread::sleep_for(1ms);
				continue;
			}

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			postOver.msgProtocol = ev.type;
			postOver.objType = ev.objType;
			//postOver.roomID = ev.roomID;

			switch (ev.objType)
			{
				case server::OBJECT_TYPE::PLAYER:
				{
					PostQueuedCompletionStatus(m_iocp, 1, ev.playerID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
				default:
				{
					PostQueuedCompletionStatus(m_iocp, 1, ev.objID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
			}
		}
	}

	void MessageHandler::AddRemoveThread()
	{
		using namespace std::chrono_literals;

		while (true)
		{
			TIMER_EVENT ev;
			auto currentTime{ CURRENT_TIME };
			bool success{ m_addRemoveQueue.try_pop(ev) };

			if (success == false)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}

			if (ev.wakeUpTime > currentTime)
			{
				m_addRemoveQueue.push(ev);
				std::this_thread::sleep_for(1ms);
				continue;
			}

			network::OVERLAPPEDEX postOver{ network::COMPLETION::BROADCAST };
			postOver.msgProtocol = ev.type;
			postOver.objType = ev.objType;
			//postOver.roomID = ev.roomID;

			switch (ev.objType)
			{
				case server::OBJECT_TYPE::PLAYER:
				{
					PostQueuedCompletionStatus(m_iocp, 1, ev.playerID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
				default:
				{
					PostQueuedCompletionStatus(m_iocp, 1, ev.objID, &postOver.over);
					std::this_thread::sleep_for(1ms);
				}
				break;
			}
		}
	}

	void MessageHandler::ExecuteMessage()
	{
		int32_t size{ m_recvQueueSize };

		if (size == 0)
			return;

		std::queue<Message> queue;

		for (int32_t i = 0; i < size;)
		{
			bool empty{ m_recvQueue.empty() };

			if (empty == true)
				return;

			Message msg{};
			bool success{ m_recvQueue.try_pop(msg) };

			if (success == false)
				continue;

			queue.push(msg);
			--m_recvQueueSize;
			++i;
		}

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
					TIMER_EVENT ev{ ProtocolID::AU_LOGIN_ACK, msg.playerID };
					PushSendMessage(ev);
				}
				break;
				case ProtocolID::AU_LOGOUT_REQ:
				{
					auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

					for (auto& playerObj : playerObjects)
					{
						auto player = dynamic_cast<Player*>(playerObj);

						if (player->GetID() == msg.playerID)
						{
							// 용섭 클라이언트 로그아웃
							Logout(msg.playerID, msg.roomID, player);

							break;
						}
					}
				}
				break;
#pragma endregion
#pragma region [MY]
				case ProtocolID::MY_ADD_ANIMATE_OBJ_REQ:
				{
					CreatePlayer(msg);
				}
				break;
				case ProtocolID::MY_KEYINPUT_REQ:
				{
					SetKeyInput(msg);
				}
				break;
				case ProtocolID::MY_ANI_PLAY_TIME_REQ:
				{
					SetPlayerAniPlayTime(msg);
				}
				break;
				case ProtocolID::MY_ANI_END_REQ:
				{
					SetAniEndFlag(msg);
				}
				break;
				case ProtocolID::MY_CAMERA_LOOK_REQ:
				{
					SetPlayerCameraLook(msg);
				}
				break;
				case ProtocolID::MY_PLAYER_LOOK_REQ:
				{
					SetPlayerLook(msg);
				}
				break;
#pragma endregion
#pragma region [WR]
#pragma endregion
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

	void MessageHandler::PushSendMessage(TIMER_EVENT& ev)
	{
		m_sendQueue.push(ev);
	}

	void MessageHandler::PushTransformMessage(TIMER_EVENT& ev)
	{
		m_transformQueue.push(ev);
	}

	void MessageHandler::PushAddRemoveMessage(TIMER_EVENT& ev)
	{
		m_addRemoveQueue.push(ev);
	}

	int32_t MessageHandler::NewObjectID()
	{
		//bool issueNewID{ m_reusableObjectID.empty() };

		//// 재사용 가능 id가 없으면 최고 숫자 발급
		//if (issueNewID == true)
		//	return (m_objectsNum++) + 100;

		//int32_t newID{ -1 };

		//// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
		//while (true)
		//{
		//	bool issueReuseID{ m_reusableObjectID.try_pop(newID) };

		//	if (issueReuseID == true)
		//	{
		//		++m_objectsNum;
		//		return newID;
		//	}
		//}

		++m_objectsNum;
		return m_objectIDOffest++;
	}

	void MessageHandler::RemoveObject()
	{
		//m_reusableObjectID.push(objID);
		--m_objectsNum;
	}

#pragma region [EXECUTE METHOD]
	void MessageHandler::Login(int32_t playerID, Player* player)
	{
		int32_t roomID{};
		if (game::CRoomManager::GetInstance()->IsRoomCreated() == false)
			roomID = game::CRoomManager::GetInstance()->CreateRoom();

		game::CRoomManager::GetInstance()->Enter(roomID, player);

		TIMER_EVENT ev{ ProtocolID::AU_LOGIN_ACK, playerID };
		ev.roomID = roomID;

		PushSendMessage(ev);
	}

	void MessageHandler::Logout(int32_t playerID, int32_t roomID, Player* player)
	{
		//game::CRoomManager::GetInstance()->Exit(roomID, player);
		m_objMgr->RemoveGameObjectFromLayer(L"Layer_Player", player);

		TIMER_EVENT ev{ ProtocolID::AU_LOGOUT_ACK, playerID };
		ev.roomID = roomID;

		PushSendMessage(ev);
	}

	void MessageHandler::CreatePlayer(Message& msg)
	{
		if (msg.objType == server::OBJECT_TYPE::PLAYER)
		{
			//Player* player{ m_objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(1500.f + msg.playerID * 50.f, 100.f, -1500.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };
			//Player* player{ m_objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(1722.f + msg.playerID * 50.f, 200.f, -1300.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };
			Player* player{ m_objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.playerID, Vec3(0.f + msg.playerID * 50.f, 200.f, -1300.f), Quat(0, 0, 0, 1), Vec3(50.f, 50.f, 50.f)) };

			std::wstring name{};

			switch (msg.fbxType)
			{
				case server::FBX_TYPE::NANA:
				{
					player->SetName(L"Nana");
				}
				break;
				case server::FBX_TYPE::MISTIC:
				{
					player->SetName(L"Mistic");
				}
				break;
				case server::FBX_TYPE::CARMEL:
				{
					player->SetName(L"Carmel");
				}
				break;
				default:
				break;
			}

			player->SetObjectType(msg.objType);
			player->SetFBXType(msg.fbxType);
		}

		TIMER_EVENT ev{ ProtocolID::WR_ADD_ANIMATE_OBJ_ACK, msg.playerID };
		ev.objType = msg.objType;

		PushSendMessage(ev);
	}

	void MessageHandler::SetKeyInput(Message& msg)
	{
		auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (auto& playerObject : playerObjects)
		{
			auto player{ dynamic_cast<Player*>(playerObject) };

			if (player == nullptr)
				continue;

			if (player->GetID() == msg.playerID)
			{
				player->GetController()->KeyboardReceive(msg.keyInput);
				break;
			}
		}
	}

	void MessageHandler::SetPlayerAniPlayTime(Message& msg)
	{
		if (msg.objType == server::OBJECT_TYPE::PLAYER)
		{
			auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

			for (auto& playerObj : playerObjects)
			{
				auto player{ dynamic_cast<Player*>(playerObj) };

				if (player == nullptr)
					continue;

				if (player->GetID() == msg.playerID)
				{
					player->SetAniPlayTime(msg.aniPlayTime);
					break;
				}
			}
		}
		else if (msg.objType == server::OBJECT_TYPE::BOSS)
		{
			auto monsterObjects{ m_objMgr->GetLayer(L"Layer_Monster")->GetGameObjects() };

			for (auto& monsterObject : monsterObjects)
			{
				auto monster{ dynamic_cast<Monster*>(monsterObject) };

				if (monster == nullptr)
					continue;

				if (monster->GetID() == msg.objID)
				{
					monster->SetAniPlayTime(msg.aniPlayTime);
					break;
				}
			}
		}
	}

	void MessageHandler::SetAniEndFlag(Message& msg)
	{
		if (msg.objType == server::OBJECT_TYPE::PLAYER)
		{
			auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

			for (auto& playerObj : playerObjects)
			{
				auto player{ dynamic_cast<Player*>(playerObj) };

				if (player == nullptr)
					continue;

				if (player->GetID() == msg.objID)
				{
					player->SetAniEndFlag(true);
					break;
				}
			}
		}
		else if (msg.objType == server::OBJECT_TYPE::BOSS)
		{
			auto monsterObjects{ m_objMgr->GetLayer(L"Layer_Monster")->GetGameObjects() };

			for (auto& monsterObject : monsterObjects)
			{
				auto monster{ dynamic_cast<Monster*>(monsterObject) };

				if (monster == nullptr)
					continue;

				if (monster->GetID() == msg.objID)
				{
					monster->SetAniEndFlag(true);
					break;
				}
			}
		}
	}

	void MessageHandler::SetPlayerCameraLook(Message& msg)
	{
		auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (auto& playerObj : playerObjects)
		{
			auto player{ dynamic_cast<Player*>(playerObj) };

			if (player == nullptr)
				continue;

			if (player->GetID() == msg.playerID)
			{
				player->SetControllerCameraLook(msg.cameraLook);
				break;
			}
		}
	}

	void MessageHandler::SetPlayerLook(Message& msg)
	{
		auto playerObjects{ m_objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };

		for (auto& playerObj : playerObjects)
		{
			auto player{ dynamic_cast<Player*>(playerObj) };

			if (player == nullptr)
				continue;

			if (player->GetID() == msg.playerID)
			{
				player->SetPlayerQuat(msg.playerLook);
				break;
			}
		}
	}

#pragma endregion
}