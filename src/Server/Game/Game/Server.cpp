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
		for (int i = 0; i < 4; ++i)
		{
			m_workerThreads.emplace_back(&CServer::WorkerThread, this);
		}

		for (auto& thread : m_workerThreads)
		{
			thread.join();
		}
	}

	void CServer::WorkerThread()
	{
		DWORD bytes;
		ULONG_PTR id;
		network::OVERLAPPEDEX* pOverEx{ nullptr };

		while (true)
		{
			if (GetQueuedCompletionStatus(m_iocp, &bytes, &id, reinterpret_cast<LPOVERLAPPED*>(&pOverEx), INFINITE) == FALSE)
			{
				if (pOverEx->type == network::COMPLETION::ACCEPT)
				{
					ErrorDisplay(L"Accept error");
				}
				else
				{
					ErrorDisplay(L"GQCS Error on client[" + std::to_wstring(id) + L"]");
					Disconnect(id);

					if (pOverEx->type == network::COMPLETION::SEND)
					{
						ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
						pOverEx = nullptr;
					}

					continue;
				}
			}

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
			}
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

	void CServer::Recv(ULONG_PTR id, DWORD bytes, network::OVERLAPPEDEX* pOverEx)
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
			packetSize = packet.GetDataSize();

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

	void CServer::Send(ULONG_PTR id, DWORD bytes, network::OVERLAPPEDEX* pOverEx)
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

	void CServer::Disconnect(ULONG_PTR id)
	{
		if (m_sessions[id]->GetState() == STATE::FREE)
		{
			std::cout << std::format("Already disconnected\n");
			return;
		}

		m_sessions[id]->Reset();
		m_reusableID.push(id);

		std::cout << std::format("session[{}] disconnected\n", id);
	}

	void CServer::ProcessPacket(ULONG_PTR id, network::CPacket& packet)
	{
		// 프로토콜 종류 읽기
		ProtocolID packetType{ packet.Read<ProtocolID>() };

		// 프로토콜의 범위에 따라서 1차적으로 패킷의 종류를 구분하여
		// 패킷 종류에 맞는 프로세스로 이동
		if (ProtocolID::PROTOCOL_AU_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_MY_BEGIN)
		{
			// 인증 프로토콜의 패킷 프로세스 처리
			ProcessAUPacket(id, packet, packetType);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= packetType and
				 packetType < ProtocolID::PROTOCOL_WR_BEGIN)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessMYPacket(id, packet, packetType);
		}
		else if (ProtocolID::PROTOCOL_WR_BEGIN <= packetType and
				 packetType < ProtocolID::PROTOCOL_BATTLE_BEGIN)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessWRPacket(id, packet, packetType);
		}
	}

	void CServer::ProcessAUPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				Login(id, packet);

				std::cout << std::format("Login Complete\n");
			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessMYPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		auto session{ m_sessions[id] };

		switch (type)
		{
			case ProtocolID::MY_MOVE_REQ:
			{
				Accessor<uint16_t, CObject*> access;
				int32_t targetID{ packet.ReadID() };

				// 세션의 델타 타임 읽기
				float deltaTime{ packet.Read<float>() };
				// 타깃의 이동방향 읽기
				DIRECTION direction{ packet.Read<DIRECTION>() };

				std::cout << id << " : ";
				//session->SetDeltaTime(deltaTime);
				session->GetMyObject()->Move(direction, deltaTime);

				// 모든 세션에 대해 이동 패킷 전송
				for (auto& player : m_sessions)
				{
					if (player->GetID() == id)
						player->SendMovePacket(id, ProtocolID::MY_MOVE_ACK, session->GetMyObject());
					else
						player->SendMovePacket(id, ProtocolID::WR_MOVE_ACK, session->GetMyObject());
				}
			}
			break;
			case ProtocolID::MY_ROTATE_REQ:
			{
				Accessor<uint16_t, CObject*> access;

				// 세션의 델타 타임 읽기
				float deltaTime{ packet.Read<float>() };
				// 타깃의 회전방향 읽기
				ROTATION direction{ packet.Read<ROTATION>() };

				//session->SetDeltaTime(deltaTime);
				session->GetMyObject()->Rotate(direction, deltaTime);

				// 모든 세션에 대해 회전 패킷 전송
				for (auto& player : m_sessions)
				{
					if (player->GetID() == id)
						player->SendMovePacket(id, ProtocolID::MY_ROTATE_ACK, session->GetMyObject());
					else
						player->SendMovePacket(id, ProtocolID::WR_ROTATE_ACK, session->GetMyObject());
				}
			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessWRPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		auto session{ m_sessions[id] };

		switch (type)
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

	void CServer::ProcessBTPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		// TODO : 추후 전투 관련 패킷 프로세스 처리
	}

	void CServer::ProcessITPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		// TODO : 추후 아이템 관련 패킷 프로세스 처리
	}

	void CServer::ProcessCMPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		// TODO : 추후 커뮤니티(eg. 채팅) 관련 패킷 프로세스 처리
	}

	void CServer::ProcessTTPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		// TODO : 추후 테스트 관련 패킷 프로세스 처리
	}

	void CServer::Login(ULONG_PTR id, network::CPacket& packet)
	{
		// Access<T, U> == tbb::concurrent_hash_map<T, U>::accessor
		// concurrent_hash_map에 접근하기 위한 일종의 shared pointer
		// concurrent_hash_map의 정보에 안전하게 접근하기 위해서는 사용 필수
		//Accessor<uint16_t, CObject*> access;
		auto session{ m_sessions[id] };
		
		// 세션의 델타 타임 읽기
		float deltaTime{ packet.Read<float>() };

		// 해당 세션의 델타 타임 설정
		//session->SetDeltaTime(deltaTime);
		session->SetState(STATE::INGAME);
		session->SetPos((m_activeSessionNum - 1) * 25.f, 0.f, 300.f);

		auto pObject{ session->GetMyObject() };

		//if (m_objects.insert(access, id) == true)
		//{
		//	access->second = pObject;
		//}

		session->SendLoginPacket(id, pObject);

		for (auto& player : m_sessions)
		{
			// 접속 중이지 않은 세션은 패스
			if (player->GetState() != STATE::INGAME)
				continue;

			// 다른 클라이언트에 오브젝트 추가 명령을 보내는 것이므로 자기 자신 제외
			if (player->GetID() == id)
				continue;

			session->SendAddPacket(player->GetID(), player->GetMyObject());
			// 해당 클라이언트에 로그인 완료 패킷 전송
			player->SendAddPacket(id, pObject);
		}
	}
}