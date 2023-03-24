#include "pch.h"
#include "Network.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <Timer.h>
#include <Resources.h>
#include <MeshData.h>
#include <Animator.h>
#include <MeshRenderer.h>

#include "Monster_Script.h"

namespace network
{
#pragma region [PUBLIC]
	CNetwork::CNetwork() :
		m_iocp{ INVALID_HANDLE_VALUE },
		m_socket{ INVALID_SOCKET },
		m_serverKey{ 99999 },
		m_pRecvPacket{ nullptr },
		m_remainSize{ 0 },
		m_id{ 0 },
		m_myObject{ nullptr }
	{
	}

	CNetwork::~CNetwork()
	{
		closesocket(m_socket);
		WSACleanup();
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

	void CNetwork::EndThreadProcess()
	{
		// 스레드 종료를 위한 QUIT 명령을 PQCS로 작업 예약
		OVERLAPPEDEX overEx{ network::COMPLETION::QUIT };

		PostQueuedCompletionStatus(m_iocp, 1, m_serverKey, &overEx.over);
	}

	void CNetwork::WaitForThreadTermination()
	{
		m_networkThread.join();
	}
#pragma endregion

#pragma region [PRIVATE]
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

	void CNetwork::Recv(DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		// 기존에 남아있던 패킷 + 새로 받은 패킷을 합친 사이즈
		int32_t remainSize{ static_cast<int32_t>(bytes) + m_remainSize };
		int32_t packetSize{};

		// CPacket 클래스에 받은 데이터 저장
		m_packet.SetData(pOverEx->data);
		m_pRecvPacket = pOverEx->data;

		while (remainSize > 0)
		{
			// 헤더(클라이언트 id + 패킷 전체 사이즈)를 뺀 순수 데이터 사이즈
			packetSize = m_packet.GetDataSize();

			if (packetSize > remainSize)
				break;

			ProcessPacket();

			// 패킷의 잘린 부분으로 포인터 이동
			m_pRecvPacket += packetSize;
			m_packet.SetData(m_pRecvPacket);
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
#pragma endregion

#pragma region [SEND PACKET]
	void CNetwork::SendLoginPacket()
	{
		network::CPacket packet;

		auto gameObjects{ GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects() };

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_REQ);
		// 델타 타임 작성
		packet.Write<float>(DELTA_TIME);

		// 패킷 전송
		Send(packet);
		Recv();
	}

	void CNetwork::SendMovePacket(DIRECTION direction)
	{
		network::CPacket packet;

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::MY_MOVE_REQ);
		// 델타 타임 작성
		packet.Write<float>(DELTA_TIME);
		// 오브젝트 이동방향 작성
		packet.Write<DIRECTION>(direction);

		// 패킷 전송
		Send(packet);
	}

	void CNetwork::SendRotationPacket(ROTATION direction)
	{
		network::CPacket packet;

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::MY_ROTATE_REQ);
		// 델타 타임 작성
		packet.Write<float>(DELTA_TIME);
		// 오브젝트 회전방향 작성
		packet.Write<ROTATION>(direction);

		// 패킷 전송
		Send(packet);
	}

	void CNetwork::SendAniIndexPacket(int32_t index)
	{
		network::CPacket packet;

		packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		//packet.Write<float>(DELTA_TIME);
		packet.Write<int32_t>(index);

		Send(packet);
	}
#pragma endregion

#pragma region [PROCESS PACKET]
	void CNetwork::ProcessPacket()
	{
		// 프로토콜 종류 읽기
		ProtocolID packetType{ m_packet.ReadProtocol() };

		// 프로토콜의 범위에 따라서 1차적으로 패킷의 종류를 구분하여
		// 패킷 종류에 맞는 프로세스로 이동
		if (ProtocolID::PROTOCOL_AU_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_AU_END)
		{
			// 인증 프로토콜의 패킷 프로세스 처리
			ProcessAUPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_MY_END)
		{
			// 플레이어 프로토콜의 패킷 프로세스 처리
			ProcessMYPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_WR_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_WR_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_BT_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_BT_END)
		{
			ProcessBTPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_IF_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_IF_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_IT_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_IT_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_CM_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_CM_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_EC_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_EC_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_GM_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_GM_END)
		{
			ProcessWRPacket(packetType);
		}
		else if (ProtocolID::PROTOCOL_TT_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_TT_END)
		{
			ProcessWRPacket(packetType);
		}
	}

	void CNetwork::ProcessAUPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_ACK:
			{
				m_id = m_packet.ReadID();

				AddPlayer(m_id);

				std::cout << "ADD ME" << std::endl;
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
				MovePlayer(m_id);
			}
			break;
			case ProtocolID::MY_ROTATE_ACK:
			{
				// 회전할 오브젝트 인덱스를 패킷에서 읽어서 타깃으로 설정				
				RotatePlayer(m_id);
			}
			break;
			case ProtocolID::MY_ANI_ACK:
			{
				//PlayAni(m_id);
			}
			break;
			default:
			break;
		}
	}

	void CNetwork::ProcessWRPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::WR_ADD_ACK:
			{
				uint32_t id{ m_packet.ReadID() };
				AddPlayer(id);

				std::cout << "ADD REMOTE" << std::endl;
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				uint32_t id{ m_packet.ReadID() };
				int32_t index{ m_idMatch[id] };
				auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };

				GET_SINGLE(SceneManager)->GetActiveScene()->RemoveGameObject(player[index]);
				m_idMatch.erase(id);
			}
			break;
			case ProtocolID::WR_MOVE_ACK:
			{
				uint32_t id{ m_packet.ReadID() };
				
				MovePlayer(id);
			}
			break;
			case ProtocolID::WR_ROTATE_ACK:
			{
				uint32_t id{ m_packet.ReadID() };
				
				RotatePlayer(id);
			}
			break;
			case ProtocolID::WR_ANI_ACK:
			{
				uint32_t id{ m_packet.ReadID() };
				
				PlayAni(id);
			}
			break;
			default:
			break;
		}
	}

#pragma region [UNUSED]
	void CNetwork::ProcessBTPacket(ProtocolID type)
	{
		// TODO : 추후 전투 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessIFPacket(ProtocolID type)
	{
	}

	void CNetwork::ProcessITPacket(ProtocolID type)
	{
		// TODO : 추후 아이템 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessCMPacket(ProtocolID type)
	{
		// TODO : 추후 커뮤니티(eg. 채팅) 관련 패킷 프로세스 처리
	}

	void CNetwork::ProcessECPacket(ProtocolID type)
	{
	}

	void CNetwork::ProcessGMPacket(ProtocolID type)
	{
	}

	void CNetwork::ProcessTTPacket(ProtocolID type)
	{
		// TODO : 추후 테스트 관련 패킷 프로세스 처리
	}
#pragma endregion

	void CNetwork::AddPlayer(uint32_t id)
	{
		Vec3 pos{};
		pos.x = m_packet.Read<float>();
		pos.y = m_packet.Read<float>();
		pos.z = m_packet.Read<float>();

		ObjectDesc objectDesc;
		objectDesc.strName = L"Dragon" + std::to_wstring(id);
		//objectDesc.strPath = L"..\\Resources\\FBX\\Moon\\moon.fbx";
		objectDesc.strPath = L"..\\Resources\\FBX\\Dragon.fbx";
		objectDesc.vPostion = pos;
		objectDesc.vScale = Vec3(30.f, 30.f, 30.f);
		objectDesc.script = std::make_shared<Monster_Dragon>();

		std::cout << "Start loading fbx" << std::endl;
		auto start{ chrono::steady_clock::now() };
		std::shared_ptr<MeshData> meshData{ GET_SINGLE(Resources)->LoadFBX(objectDesc.strPath) };
		auto end{ chrono::steady_clock::now() };
		std::cout << "fbx load finished : " << std::chrono::duration_cast<std::chrono::seconds>(end - start) << std::endl;
		std::vector<std::shared_ptr<CGameObject>> gameObjects{ meshData->Instantiate() };

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(objectDesc.strName);
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(objectDesc.vPostion);
			gameObject->GetTransform()->SetLocalScale(objectDesc.vScale);
			gameObject->AddComponent(objectDesc.script);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);
		}

		GET_SINGLE(SceneManager)->GetActiveScene()->AddPlayer(gameObjects);

		auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };
		m_idMatch.insert(std::make_pair(id, player.size() - 2));
	}

	void CNetwork::MovePlayer(uint32_t id)
	{
		int32_t index{ m_idMatch[id] };
		auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };

		// 패킷에서 오브젝트를 렌더링할 좌표 읽기
		Vec3 pos;
		pos.x = m_packet.Read<float>();
		pos.y = m_packet.Read<float>();
		pos.z = m_packet.Read<float>();

		// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
		player[index]->GetTransform()->SetLocalPosition(pos);
	}

	void CNetwork::RotatePlayer(uint32_t id)
	{
		int32_t index{ m_idMatch[id] };
		auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };

		// 패킷에서 오브젝트를 렌더링할 좌표 읽기
		Vec3 rotation;
		rotation.x = m_packet.Read<float>();
		rotation.y = m_packet.Read<float>();
		rotation.z = m_packet.Read<float>();

		// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
		player[index]->GetTransform()->SetLocalRotation(rotation);
	}

	void CNetwork::PlayAni(uint32_t id)
	{
		int32_t index{ m_idMatch[id] };
		auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };

		int32_t aniIndex{ m_packet.Read<int32_t>() };
		player[index]->GetAnimator()->Play(aniIndex);
	}
#pragma endregion
}