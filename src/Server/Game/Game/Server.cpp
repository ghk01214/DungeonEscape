#include "pch.h"
#include "Session.h"
#include "GameInstance.h"
#include "RoomManager.h"
#include "Server.h"
#include "TimeManager.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "Player.h"
#include "CustomController.h"
#include "MapObject.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "MessageHandler.h"

namespace game
{
	CServer::CServer() :
		m_iocp{ INVALID_HANDLE_VALUE },
		m_socket{ INVALID_SOCKET },
		m_key{ 99999 },
		m_activeSessionNum{ 0 },
		m_userID{ 0 }
	{
		// 클래스 생성 시 빈 세션 생성
		for (int32_t i = 0; i < MAX_USER; ++i)
		{
			m_sessions[i] = new CSession{ new Player{} };
		}
	}

	CServer::~CServer()
	{
		closesocket(m_socket);
		WSACleanup();

		if (m_sessions.empty() == false)
		{
			for (auto& client : m_sessions)
			{
				delete client;
				client = nullptr;
			}
		}

		GameInstance::GetInstance()->DestroyInstance();
		m_gameInstance = nullptr;
	}

	void CServer::Run()
	{
		Initialize();
		Accept();
		CreateThread();
	}

	// 서버 기본 초기화
	void CServer::Initialize()
	{
		if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
		{
			ErrorQuit(L"WSAStartup fuction error");
		}

		if (m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, NULL);
			m_iocp == INVALID_HANDLE_VALUE)
		{
			ErrorQuit(L"IOCP Handle Creation Failed");
		}

		m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

		sockaddr_in serverAddr;
		ZeroMemory(&serverAddr, sizeof(sockaddr_in));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(GAME_SERVER_PORT);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket), m_iocp, m_key, 0);

		if (bind(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		{
			ErrorQuit(L"bind function error");
		}

		if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			ErrorQuit(L"listen function error");
		}

		m_roomManager = CRoomManager::GetInstance();
		m_gameInstance = GameInstance::GetInstance();
		MessageHandler::GetInstance()->SetIOCPHandle(m_iocp);

		m_roomManager->Init();
		m_gameInstance->Init();
	}

	// accept 등록
	void CServer::Accept()
	{
		SOCKET clientSocket{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };

		network::OVERLAPPEDEX acceptEx;
		acceptEx.type = network::COMPLETION::ACCEPT;
		acceptEx.wsa.buf = reinterpret_cast<char*>(clientSocket);

		AcceptEx(m_socket, clientSocket, acceptEx.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &acceptEx.over);
	}

	// Worker thread용 멀티스레드 생성
	void CServer::CreateThread()
	{
		for (int32_t i = 0; i < 3; ++i)
		{
			m_workerThreads.emplace_back(&CServer::WorkerThread, this);
		}

		m_gameThreads = std::thread{ &CServer::GameThread, this };

		for (auto& thread : m_workerThreads)
		{
			thread.join();
		}

		m_gameThreads.join();
	}

	void CServer::WorkerThread()
	{
		DWORD bytes;
		ULONG_PTR clientID;
		//pOverEx{ nullptr };

		while (true)
		{
			OVERLAPPED* over{ nullptr };
			BOOL ret{ GetQueuedCompletionStatus(m_iocp, &bytes, &clientID, &over, INFINITE) };
			network::OVERLAPPEDEX* pOverEx{ reinterpret_cast<network::OVERLAPPEDEX*>(over) };

			if (ret == FALSE)
			{
				if (pOverEx->type == network::COMPLETION::ACCEPT)
				{
					ErrorDisplay(L"Accept error");
				}
				else
				{
					ErrorDisplay(L"GQCS Error on session[" + std::to_wstring(clientID) + L"] :");
					Disconnect(static_cast<uint32_t>(clientID));

					if (pOverEx->type == network::COMPLETION::SEND)
					{
						ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
						pOverEx = nullptr;
						//if (pOverEx != nullptr)
						//	delete pOverEx;
					}

					continue;
				}
			}

			if (bytes == 0 and (pOverEx->type == network::COMPLETION::RECV or pOverEx->type == network::COMPLETION::SEND))
			{
				Disconnect(static_cast<uint32_t>(clientID));

				if (pOverEx->type == network::COMPLETION::SEND)
				{
					ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
					pOverEx = nullptr;
					//if (pOverEx != nullptr)
					//	delete pOverEx;
				}

				continue;
			}

			uint32_t id{ static_cast<uint32_t>(clientID) };

			switch (pOverEx->type)
			{
				case network::COMPLETION::ACCEPT:
				{
					AcceptClient(pOverEx);
				}
				break;
				case network::COMPLETION::RECV:
				{
					Recv(id, bytes, pOverEx);
				}
				break;
				case network::COMPLETION::SEND:
				{
					Send(id, bytes, pOverEx);
				}
				break;
				case network::COMPLETION::BROADCAST:
				{
					BroadcastResult(id, pOverEx);
				}
				default:
				break;
			}
		}
	}

	void CServer::GameThread()
	{
		while (true)
		{
			double timeDelta{ TimeManager::GetInstance()->GetElapsedTime() };
			m_gameInstance->Update(timeDelta);
			m_gameInstance->LateUpdate(timeDelta);
			MessageHandler::GetInstance()->SendPacketMessage();
		}
	}

	void CServer::AcceptClient(network::OVERLAPPEDEX* pOverEx)
	{
		SOCKET clientSocket{ reinterpret_cast<SOCKET>(pOverEx->wsa.buf) };

		if (int32_t id{ NewPlayerID() }; id != -1)
		{
			m_sessions[id]->m_prevRemain = 0;
			m_sessions[id]->SetSocket(clientSocket);
			m_sessions[id]->SetID(id);
			m_sessions[id]->SetState(STATE::ACCEPTED);

			std::cout << std::format("session[{}] accepted\n", id);

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_iocp, id, 0);

			m_sessions[id]->Recv();

			clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		}
		else
		{
			std::cout << std::format("Max user exceeded!\n");
		}

		ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
		pOverEx->wsa.buf = reinterpret_cast<char*>(clientSocket);

		AcceptEx(m_socket, clientSocket, pOverEx->data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &pOverEx->over);
	}

	void CServer::Recv(uint32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		if (bytes == 0)
		{
			Disconnect(id);
			return;
		}

		int32_t remainSize{ static_cast<int32_t>(bytes) + m_sessions[id]->m_prevRemain };
		int32_t packetSize{};

		char* pPacket{ pOverEx->data };
		network::CPacket packet{};
		packet.SetData(pPacket);

		while (remainSize > 0)
		{
			packetSize = packet.GetPacketSize();

			if (packetSize > remainSize)
				break;

			ProcessPacket(id, packet);

			pPacket += packetSize;
			packet.SetData(pPacket);
			remainSize -= packetSize;
		}

		m_sessions[id]->m_prevRemain = remainSize;

		if (remainSize > 0)
		{
			std::memcpy(pOverEx->data, pPacket, remainSize);
		}

		m_sessions[id]->Recv();
	}

	void CServer::Send(uint32_t id, DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		//if (bytes == 0)
		//{
		//	Disconnect(id);
		//}

		ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
		pOverEx = nullptr;
		//if (pOverEx != nullptr)
		//	delete pOverEx;
	}

	int32_t CServer::NewPlayerID()
	{
		// 현재 활성화된 세션의 개수가 수용 가능한 유저수를 초과하면 아이디 발급 거부
		if (m_activeSessionNum >= MAX_USER)
		{
			std::cout << std::format("MAX USER FULL\n");
			return -1;
		}

		// 활성 세션수 증가
		++m_activeSessionNum;

		bool issueNewID{ m_reusableID.empty() };

		// 재사용 가능 id가 없으면 최고 숫자 발급
		if (issueNewID == true)
			return m_userID++;

		int32_t newID{ -1 };

		// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
		while (true)
		{
			bool issueReuseID{ m_reusableID.try_pop(newID) };

			if (issueReuseID == true)
				return newID;
		}
	}

	void CServer::Disconnect(uint32_t id)
	{
		for (auto& session : m_sessions)
		{
			if (session->GetState() != STATE::INGAME)
				continue;

			if (session->GetID() == id)
				continue;

			session->SendRemovePacket(id);
		}

		m_sessions[id]->Reset();
		m_reusableID.push(id);
		--m_activeSessionNum;

		std::cout << std::format("session[{}] disconnected\n", id);
	}

	void CServer::ProcessPacket(uint32_t id, network::CPacket& packet)
	{
		// 프로토콜 종류 읽기
		ProtocolID protocol{ packet.ReadProtocol() };

		// 프로토콜의 범위에 따라서 1차적으로 패킷의 종류를 구분하여
		// 패킷 종류에 맞는 프로세스로 이동
		if (ProtocolID::PROTOCOL_AU_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_AU_END)
		{
			// 인증 프로토콜의 패킷 프로세스 처리
			ProcessAUPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_MY_END)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessMYPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_WR_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_WR_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_BT_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_BT_END)
		{
			ProcessBTPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_IF_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_IF_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_IT_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_IT_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_CM_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_CM_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_EC_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_EC_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_GM_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_GM_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
		else if (ProtocolID::PROTOCOL_TT_BEGIN <= protocol and
			protocol < ProtocolID::PROTOCOL_TT_END)
		{
			ProcessWRPacket(id, packet, protocol);
		}
	}

	void CServer::ProcessAUPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		switch (protocol)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				Login(id, packet);

				std::cout << std::format("session[{}] login Complete\n", id);
			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessMYPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		auto session{ m_sessions[id] };

		switch (protocol)
		{
			case ProtocolID::MY_ADD_REQ:
			{
				ProtocolID protocol{ packet.ReadProtocol() };
				InputCommandMessage(id, protocol);
			}
			break;
			case ProtocolID::MY_KEYINPUT_REQ:
			{
				// 타깃의 이동방향 읽기
				auto keyInput{ packet.Read<unsigned long>() };
				auto playerLayer{ ObjectManager::GetInstance()->GetLayer(L"Layer_Player") };

				for (auto& playerObject : playerLayer->GetGameObjects())
				{
					auto player{ dynamic_cast<Player*>(playerObject) };

					if (player->GetPlayerID() == id)
					{
						auto customController{ player->GetComponent<CustomController>(L"CustomController") };
						customController->KeyboardReceive(keyInput);
					}
				}
			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessWRPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		auto session{ m_sessions[id] };

		switch (protocol)
		{
			case ProtocolID::WR_ADD_REQ:
			{

			}
			break;
			case ProtocolID::WR_REMOVE_REQ:
			{

			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessBTPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		// TODO : 추후 전투 관련 패킷 프로세스 처리
	}

	void CServer::ProcessIFPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
	}

	void CServer::ProcessITPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		// TODO : 추후 아이템 관련 패킷 프로세스 처리
	}

	void CServer::ProcessCMPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		// TODO : 추후 커뮤니티(eg. 채팅) 관련 패킷 프로세스 처리
	}

	void CServer::ProcessECPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
	}

	void CServer::ProcessGMPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
	}

	void CServer::ProcessTTPacket(uint32_t id, network::CPacket& packet, ProtocolID protocol)
	{
		// TODO : 추후 테스트 관련 패킷 프로세스 처리
	}

	void CServer::Login(uint32_t id, network::CPacket& packet)
	{
		auto session{ m_sessions[id] };

		session->SetState(STATE::INGAME);

		// 용섭 : Player 오브젝트 새로 생성
		//auto objMgr{ ObjectManager::GetInstance() };
		//Player* player{ objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5)) };
		//player->SetPlayerID(id);
		//session->SetObject(player);
		//session->SendLoginPacket(player);
		ProtocolID protocol{ packet.ReadProtocol() };
		InputCommandMessage(id, protocol);

		//for (auto& client : m_sessions)
		//{
		//	if (client->GetState() != STATE::INGAME)
		//		continue;
		//
		//	if (client->GetID() == id)
		//		continue;
		//
		//	//client->SendAddPacket(id, player);
		//	session->SendAddPacket(client->GetID(), client->GetMyObject());
		//}
	}

	void CServer::BroadcastResult(int32_t id, network::OVERLAPPEDEX* over)
	{
		auto session{ m_sessions[id] };

		if (session->GetState() != STATE::INGAME)
			return;

		auto objMgr{ ObjectManager::GetInstance() };
		auto playerObjects{ objMgr->GetLayer(L"Layer_Player")->GetGameObjects() };
		auto mapObjects{ objMgr->GetLayer(L"Layer_Map")->GetGameObjects() };

		switch (over->msgProtocol)
		{
			case ProtocolID::AU_LOGIN_ACK:
			{
				Player* playerObj{ nullptr };

				for (auto& player : playerObjects)
				{
					playerObj = dynamic_cast<Player*>(player);

					if (playerObj->GetPlayerID() == id)
						break;
				}

				session->SetObject(playerObj);
				session->SendLoginPacket(playerObj);

				for (auto& client : m_sessions)
				{
					if (client->GetState() != STATE::INGAME)
						continue;

					if (client->GetID() == id)
						continue;

					client->SendAddPacket(id, playerObj);
					session->SendAddPacket(client->GetID(), client->GetMyObject());
				}

				for (auto& client : m_sessions)
				{
					if (client->GetState() != STATE::INGAME)
						continue;

					for (auto& obj : mapObjects)
					{
						session->SendAddPacket(obj->GetID(), obj);
					}
				}
			}
			break;
			case ProtocolID::MY_TRANSFORM_ACK:
			case ProtocolID::WR_TRANSFORM_ACK:
			{
				for (auto& player : playerObjects)
				{
					auto pl{ dynamic_cast<Player*>(player) };

					if (pl->GetPlayerID() == id)
						session->SendTransformPacket(id, ProtocolID::MY_TRANSFORM_ACK, player);
					else
						session->SendTransformPacket(pl->GetPlayerID(), ProtocolID::WR_TRANSFORM_ACK, player);
				}

				for (auto& object : mapObjects)
				{
					auto map{ dynamic_cast<MapObject*>(object) };

					if (map->GetRequireFlagTransmit() == true)		//위치갱신에 따라 패킷전송 플래그가 켜져있는가?
					{
						session->SendTransformPacket(object->GetID(), ProtocolID::WR_TRANSFORM_ACK, object);	//트랜스폼 갱신
						map->SetRequireFlagTransmit(false);														//플래그 체크
					}
				}
			}
			break;
			case ProtocolID::WR_ADD_ACK:
			{
				GameObject* object{ nullptr };

				for (auto& obj : mapObjects)
				{
					if (obj->GetID() != over->targetID)
						continue;

					object = obj;
				}

				for (auto& client : m_sessions)
				{
					if (client->GetState() != STATE::INGAME)
						continue;

					client->SendAddPacket(over->targetID, object);
				}
			}
			break;
		}
	}

	void CServer::InputCommandMessage(int32_t id, ProtocolID type)
	{
		MessageHandler::GetInstance()->InsertRecvMessage(id, type);
	}
}