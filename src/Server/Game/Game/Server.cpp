#include "pch.h"
#include <OVERLAPPEDEX.h>
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
			m_workerThreads.push_back(std::thread{ &CServer::WorkerThread, this });
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
			std::cout << "Accepted" << std::endl;

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

	int CServer::NewPlayerID()
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
			return;
		}

		m_sessions[id]->Reset();
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
				 packetType < ProtocolID::PROTOCOL_BATTLE_BEGIN)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessMYPacket(id, packet, packetType);
		}
	}

	void CServer::ProcessAUPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				// 전체 오브젝트 개수 읽기
				uint16_t objNum{ packet.Read<uint16_t>() };
				// Access<T, U> == tbb::concurrent_hash_map<T, U>::accessor
				// concurrent_hash_map에 접근하기 위한 일종의 shared pointer
				// concurrent_hash_map의 정보에 안전하게 접근하기 위해서는 사용 필수
				Accessor<uint16_t, CObject*> access;

				for (int32_t i = 0; i < objNum; ++i)
				{
					// 오브젝트 인덱스 읽기
					uint16_t objIndex{ packet.Read<uint16_t>() };

					CObject* obj{ new CObject{} };
					// 오브젝트 좌표 읽기
					float x{ packet.Read<float>() };
					float y{ packet.Read<float>() };
					float z{ packet.Read<float>() };

					// 읽은 오브젝트 좌표 설정
					obj->SetPos(x, y, z);

					// 오브젝트 회전각 읽기
					x = packet.Read<float>();
					y = packet.Read<float>();
					z = packet.Read<float>();

					// 읽은 오브젝트 회전각 설정
					obj->SetRotate(x, y, z);

					// 설정한 오브젝트를 container에 삽입
					if (m_objects.insert(access, objIndex))
					{
						access->second = obj;
					}
				}

				// 해당 세션의 메인 타킷 인덱스 설정
				m_sessions[id]->SetTargetNum(m_activeSessionNum - 1);
				// 해당 클라이언트에 로그인 완료 패킷 전송
				m_sessions[id]->SendLoginPacket();

				std::cout << "Login Complete" << std::endl;
			}
			break;
			default:
			break;
		}
	}

	void CServer::ProcessMYPacket(ULONG_PTR id, network::CPacket& packet, ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::MY_MOVE_REQ:
			{
				Accessor<uint16_t, CObject*> access;
				// 이동할 타깃 인덱스 읽기
				uint16_t targetNum{ packet.Read<uint16_t>() };

				// 이동할 타깃을 container에서 검색
				m_objects.find(access, targetNum);

				// 타깃의 이동방향 읽기
				DIRECTION direction{ packet.Read<DIRECTION>() };
				// 타깃 이동
				access->second->Move(direction);

				// 모든 세션에 대해 이동 패킷 전송
				for (int32_t i = 0; i < m_activeSessionNum; ++i)
				{
					m_sessions[i]->SendMovePacket(id, targetNum, access->second);
				}
			}
			break;
			case ProtocolID::MY_ROTATE_REQ:
			{
				Accessor<uint16_t, CObject*> access;
				// 회전할 타깃 인덱스 읽기
				uint16_t targetNum{ packet.Read<uint16_t>() };

				// 회전할 타깃을 container에서 검색
				m_objects.find(access, targetNum);

				// 타깃의 회전방향 읽기
				ROTATION direction{ packet.Read<ROTATION>() };
				// 타깃 회전
				access->second->Rotate(direction);

				// 모든 세션에 대해 회전 패킷 전송
				for (int32_t i = 0; i < m_activeSessionNum; ++i)
				{
					m_sessions[i]->SendRotatePacket(id, targetNum, access->second);
				}
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
}