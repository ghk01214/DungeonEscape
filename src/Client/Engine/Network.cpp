#include "pch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Network.h"
#include "Transform.h"
#include <iostream>

namespace network
{
	CNetwork::CNetwork() :
		m_iocp{ INVALID_HANDLE_VALUE },
		m_socket{ INVALID_SOCKET },
		m_serverKey{ 99999 },
		m_recvPacket{ nullptr },
		m_remainSize{ 0 },
		m_myTarget{ 0 }
	{
		// 서버와 커넥트
		Connect();
	}

	CNetwork::~CNetwork()
	{
		closesocket(m_socket);
		WSACleanup();

		// 여전히 문제 있음, 개선 필요
		m_networkThread.join();
	}

	// 기본 초기화 작업
	void CNetwork::Connect()
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
		InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket), m_iocp, m_serverKey, 0);

		if (WSAConnect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr), 0, 0, 0, 0) == SOCKET_ERROR)
		{
			ErrorQuit(L"Connection Failed");
		}

		// GQCS를 비동기로 돌릴 네트워크 스레드 생성
		m_networkThread = std::thread{ &CNetwork::ProcessThread, this };
	}

	void CNetwork::ProcessThread()
	{
		DWORD bytes;
		ULONG_PTR id;
		OVERLAPPEDEX* pOverEx{ nullptr };

		while (true)
		{
			if (GetQueuedCompletionStatus(m_iocp, &bytes, &id, reinterpret_cast<LPOVERLAPPED*>(&pOverEx), INFINITE) == 0)
			{
				ErrorQuit(L"GetQueuedCompletionStatus Error");
			}

			// COMPLETION 종류에 따른 작업 구분
			switch (pOverEx->type)
			{
				case COMPLETION::RECV:
				{
					Recv(bytes, pOverEx);
				}
				break;
				case COMPLETION::SEND:
				{
					Send(bytes, pOverEx);
				}
				break;
				case COMPLETION::QUIT:
				{
					return;
				}
				default:
				break;
			}
		}
	}

	void CNetwork::EndThread()
	{
		// 스레드 종료를 위한 QUIT 명령을 PQCS로 작업 예약
		OVERLAPPEDEX overEx{ network::COMPLETION::QUIT };

		PostQueuedCompletionStatus(m_iocp, 1, m_serverKey, &overEx.over);
	}

	void CNetwork::Recv(DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		// 기존에 남아있던 패킷 + 새로 받은 패킷을 합친 사이즈
		int32_t remainSize{ static_cast<int32_t>(bytes) + m_remainSize };
		int32_t packetSize{};

		// CPacket 클래스에 받은 데이터 저장
		m_packet.SetData(pOverEx->data);
		m_recvPacket = pOverEx->data;

		while (remainSize > 0)
		{
			// 헤더(클라이언트 id + 패킷 전체 사이즈)를 뺀 순수 데이터 사이즈
			packetSize = m_packet.GetDataSize();

			if (packetSize > remainSize)
				break;

			ProcessPacket();

			// 패킷의 잘린 부분으로 포인터 이동
			m_recvPacket += packetSize;
			m_packet.SetData(m_recvPacket);
			// 처리한 패킷 사이즈만큼 빼기
			remainSize -= packetSize;
		}

		// 잘린 데이터를 OVERLAPPEDEX 구조체의 맨 앞으로 이동
		if (remainSize > 0)
		{
			std::memcpy(pOverEx->data, m_packet.GetPacketAddr(), remainSize);
		}

		// 다시 Recv 상태로 대기
		Recv();
	}

	void CNetwork::Send(DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
		pOverEx = nullptr;
		//delete pOverEx;
	}

#pragma region [SEND PACKET]
	void CNetwork::SendLoginPacket(std::vector<std::shared_ptr<CGameObject>>& m_gameObjects)
	{
		network::CPacket packet;

		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::AU_LOGIN_REQ);
		// 전체 오브젝트 개수 작성
		packet.Write<uint16_t>(static_cast<uint16_t>(m_gameObjects.size()));

		// 각 오브젝트의 인덱스와 좌표, 회전각 작성
		for (uint16_t i = 0; i < m_gameObjects.size(); ++i)
		{
			packet.Write<uint16_t>(i);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().x);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().y);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().z);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalRotation().x);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalRotation().y);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalRotation().z);
		}

		// 패킷 전송
		Send(packet);
	}

	void CNetwork::SendMovePacket(DIRECTION direction)
	{
		network::CPacket packet;

		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::MY_MOVE_REQ);
		// 움직일 오브젝트의 인덱스 작성
		packet.Write<uint16_t>(m_myTarget);
		// 오브젝트 이동방향 작성
		packet.Write<DIRECTION>(direction);

		// 패킷 전송
		Send(packet);
	}

	void CNetwork::SendRotationPacket(ROTATION direction)
	{
		network::CPacket packet;

		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::MY_ROTATE_REQ);
		// 회전할 오브젝트의 인덱스 작성
		packet.Write<uint16_t>(m_myTarget);
		// 오브젝트 회전방향 작성
		packet.Write<ROTATION>(direction);

		// 패킷 전송
		Send(packet);
	}
#pragma endregion

	void CNetwork::Recv()
	{
		DWORD flag{};
		ZeroMemory(&m_recvEx.over, sizeof(m_recvEx.over));

		m_recvEx.wsa.len = network::CPacket::BUFF_SIZE + m_remainSize;
		m_recvEx.wsa.buf = m_recvEx.data + m_remainSize;

		WSARecv(m_socket, &m_recvEx.wsa, 1, 0, &flag, &m_recvEx.over, nullptr);
	}

	void CNetwork::Send(network::CPacket& packet)
	{
		m_sendEx.Set(packet);

		WSASend(m_socket, &m_sendEx.wsa, 1, 0, 0, &m_sendEx.over, nullptr);
	}

	void CNetwork::ProcessPacket()
	{
		// 프로토콜 종류 읽기
		ProtocolID packetType{ m_packet.Read<ProtocolID>() };

		// 프로토콜의 범위에 따라서 1차적으로 패킷의 종류를 구분하여
		// 패킷 종류에 맞는 프로세스로 이동
		if (ProtocolID::PROTOCOL_AU_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_MY_BEGIN)
		{
			// 인증 프로토콜의 패킷 프로세스 처리
			ProcessAUPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_BATTLE_BEGIN)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessMYPacket(packetType);
		}
	}

	void CNetwork::ProcessAUPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_ACK:
			{
				// 클라이언트의 조작 오브젝트 인덱스(변동X)
				m_myTarget = m_packet.Read<uint16_t>();

				GET_SINGLE(CSceneManager)->SetTarget(m_myTarget);
			}
			break;
			default:
			break;
		}
	}

	void CNetwork::ProcessMYPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::MY_MOVE_ACK:
			{
				// 움직일 오브젝트 인덱스를 패킷에서 읽어서 타깃으로 설정
				GET_SINGLE(CSceneManager)->SetTarget(m_packet.Read<uint16_t>());

				// 패킷에서 오브젝트를 렌더링할 좌표 읽기
				Vec3 pos;
				pos.x = m_packet.Read<float>();
				pos.y = m_packet.Read<float>();
				pos.z = m_packet.Read<float>();

				// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
				GET_SINGLE(CSceneManager)->ObjectTranslationMode(pos);
			}
			break;
			// 회전은 개선이 필요(회전을 하지 않고 이동을 함)
			case ProtocolID::MY_ROTATE_ACK:
			{
				// 회전할 오브젝트 인덱스를 패킷에서 읽어서 타깃으로 설정
				GET_SINGLE(CSceneManager)->SetTarget(m_packet.Read<uint16_t>());

				// 패킷에서 오브젝트를 렌더링할 좌표 읽기
				Vec3 rotation;
				rotation.x = m_packet.Read<float>();
				rotation.y = m_packet.Read<float>();
				rotation.z = m_packet.Read<float>();

				// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
				GET_SINGLE(CSceneManager)->ObjectRotationMode(rotation);
			}
			break;
			default:
			break;
		}
	}

	void CNetwork::ProcessBTPacket(ProtocolID type)
	{
		// TODO : 추후 전투 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessITPacket(ProtocolID type)
	{
		// TODO : 추후 아이템 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessCMPacket(ProtocolID type)
	{
		// TODO : 추후 커뮤니티(eg. 채팅) 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessTTPacket(ProtocolID type)
	{
		// TODO : 추후 테스트 관련 패킷 프로세스 처리
	}
}