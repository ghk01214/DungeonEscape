#include "pch.h"
#include "Session.h"
#include "Object.h"
#include "Player.h"
#include "Server.h"

namespace game
{
	CServer::CServer() :
		m_iocp{ INVALID_HANDLE_VALUE },
		m_socket{ INVALID_SOCKET },
		m_key{ 99999 },
		m_activeSessionNum{ 0 },
		m_userID{ 0 },
		m_randomID{ 0, 999999999 }
	{
		// 클래스 생성 시 빈 세션 생성
		for (int32_t i = 0; i < MAX_USER; ++i)
		{
			m_sessions[i] = new CSession{ new CPlayer{} };
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
			m_physxThreads.emplace_back(&CServer::PhysxThread, this);
		}

		for (auto& thread : m_workerThreads)
		{
			thread.join();
		}

		for (auto& thread : m_physxThreads)
		{
			thread.join();
		}
	}

	void CServer::WorkerThread()
	{
		DWORD bytes;
		ULONG_PTR clientID;
		network::OVERLAPPEDEX* pOverEx{ nullptr };

		while (true)
		{
			if (GetQueuedCompletionStatus(m_iocp, &bytes, &clientID, reinterpret_cast<LPOVERLAPPED*>(&pOverEx), INFINITE) == FALSE)
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
					}

					continue;
				}
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
				case network::COMPLETION::JUMP:
				{
					Jump(id, pOverEx);
				}
				break;
				default:
				break;
			}
		}
	}

	void CServer::PhysxThread()
	{
		while (true)
		{

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
		if (bytes == 0)
		{
			Disconnect(id);
		}

		ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
		pOverEx = nullptr;
		//delete pOverEx;
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

		// 재사용 가능 id가 없으면 최고 숫자 발급
		if (bool issueNewID{ m_reusableID.empty() }; issueNewID == true)
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
		if (m_sessions[id]->GetState() == STATE::FREE)
		{
			std::cout << std::format("Already disconnected\n");
			return;
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
			case ProtocolID::MY_TRANSFORM_REQ:
			{
				// 세션의 델타 타임 읽기
				float deltaTime{ packet.Read<float>() };
				// 타깃의 이동방향 읽기
				uint8_t keyInput{ packet.Read<uint8_t>() };
				server::KEY_STATE keyState{ packet.Read<server::KEY_STATE>() };

				auto pl{ dynamic_cast<CPlayer*>(session->GetMyObject()) };

				std::cout << id << " : ";
				pl->Transform(keyInput, keyState, deltaTime);

				// 모든 세션에 대해 이동 패킷 전송
				for (auto& player : m_sessions)
				{
					if (player->GetState() != STATE::INGAME)
						continue;

					if (player->GetID() == id)
						player->SendTransformPacket(id, ProtocolID::MY_TRANSFORM_ACK, session->GetMyObject());
					else
						player->SendTransformPacket(id, ProtocolID::WR_TRANSFORM_ACK, session->GetMyObject());
				}

				if (pl->IsJumping() == true)
				{
					network::OVERLAPPEDEX* pOverEx{ new network::OVERLAPPEDEX };
					pOverEx->type = network::COMPLETION::JUMP;

					PostQueuedCompletionStatus(m_iocp, 1, id, &pOverEx->over);
				}
			}
			break;
			case ProtocolID::MY_ANI_REQ:
			{
				int32_t index{ packet.Read<int32_t>() };
				float aniFrame{ packet.Read<float>() };
				auto pl{ dynamic_cast<CPlayer*>(session->GetMyObject()) };

				pl->SetAniIndex(index);
				pl->SetAniFrame(aniFrame);

				for (auto& player : m_sessions)
				{
					if (player->GetState() != STATE::INGAME)
						continue;

					if (player->GetID() == id)
						player->SendAniIndexPacket(id, ProtocolID::MY_ANI_ACK, pl);
					else
						player->SendAniIndexPacket(id, ProtocolID::WR_ANI_ACK, pl);
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
		// Access<T, U> == tbb::concurrent_hash_map<T, U>::accessor
		// concurrent_hash_map에 접근하기 위한 일종의 shared pointer
		// concurrent_hash_map의 정보에 안전하게 접근하기 위해서는 사용 필수
		//Accessor<uint16_t, CObject*> access;
		auto session{ m_sessions[id] };

		// 세션의 델타 타임 읽기
		float deltaTime{ packet.Read<float>() };

		float x{ (m_activeSessionNum - 1) * 25.f };
		// 해당 세션의 델타 타임 설정
		session->SetState(STATE::INGAME);
		session->SetPos((m_activeSessionNum - 1) * 25.f, 0.f, 300.f);

		auto pObject{ session->GetMyObject() };

		auto aniIndex{ packet.Read<int32_t>() };
		dynamic_cast<CPlayer*>(pObject)->SetAniIndex(aniIndex);

		session->SendLoginPacket(id, pObject);

		for (auto& player : m_sessions)
		{
			// 접속 중이지 않은 세션은 패스
			if (player->GetState() != STATE::INGAME)
				continue;

			// 다른 클라이언트에 오브젝트 추가 명령을 보내는 것이므로 자기 자신 제외
			if (player->GetID() == id)
				continue;

			// 해당 클라이언트에 로그인 완료 패킷 전송
			player->SendAddPacket(id, pObject);
			session->SendAddPacket(player->GetID(), player->GetMyObject());
		}
	}

	void CServer::Jump(int32_t id, network::OVERLAPPEDEX* pOverEx)
	{
		auto pl{ dynamic_cast<CPlayer*>(m_sessions[id]->GetMyObject()) };

		pl->Jump();

		for (auto& player : m_sessions)
		{
			if (player->GetState() != STATE::INGAME)
				continue;

			if (player->GetID() == id)
				player->SendTransformPacket(id, ProtocolID::MY_JUMP_ACK, pl);
			else
				player->SendTransformPacket(id, ProtocolID::WR_JUMP_ACK, pl);
		}

		if (pl->IsJumping() == true)
		{
			PostQueuedCompletionStatus(m_iocp, 1, id, &pOverEx->over);
		}
	}
}