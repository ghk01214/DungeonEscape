#include "pch.h"
#include "NetworkManager.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "Resources.h"
#include "MeshData.h"
#include "Animator.h"
#include "MeshRenderer.h"
#include "Network.h"

namespace network
{
#pragma region [PUBLIC]
	void NetworkManager::Init()
	{
		m_iocp = INVALID_HANDLE_VALUE;
		m_socket = INVALID_SOCKET;
		m_serverKey = 99999;
		m_pRecvPacket = nullptr;
		m_remainSize = 0;
		m_id = 0;

		Connect();
	}

	void NetworkManager::RegisterObject(std::shared_ptr<CGameObject> object)
	{
		// 해당 오브젝트가 기존에 존재하는 오브젝트인지 검사
		// 초기값인 -1이 아니라면 등록됬다는 뜻
		if (object->GetNetwork()->GetID() != -1)
		{
			// 존재하면 넘어감
			MSG_BOX(TEXT("Failed : NetworkManager::RegisterObject - Already exist Object"));
			return;
		}

		// 0번부터 순서대로 ObjectID 부여
		uint32 objectID = m_objects.size();

		// 인자로 받은 게임 오브젝트의 네트워크 컴포넌트에 objectID 부여
		object->GetNetwork()->SetID(objectID);

		// 멤버 관리 변수에 추가
	//	m_objects.insert(std::make_pair(objectID, object));
	}

	void NetworkManager::RegisterObject(Object object)
	{
		Object objs;

		for (auto& obj : object)
		{
			// 해당 오브젝트가 기존에 존재하는 오브젝트인지 검사
			// 초기값인 -1이 아니라면 등록됬다는 뜻
			if (obj->GetNetwork()->GetID() != -1)
			{
				// 존재하면 넘어감
				MSG_BOX(TEXT("Failed : NetworkManager::RegisterObject - Already exist Object"));
				return;
			}

			// 0번부터 순서대로 ObjectID 부여
			uint32 objectID = m_objects.size();

			// 인자로 받은 게임 오브젝트의 네트워크 컴포넌트에 objectID 부여
			obj->GetNetwork()->SetID(objectID);

			objs.push_back(obj);
		}

		// 멤버 관리 변수에 추가
		//m_objects.insert(std::make_pair(objectID, objs));
	}

	// 기본 초기화 작업
	void NetworkManager::Connect()
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
		ZeroMemory(&serverAddr, sizeof(serverAddr));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(GAME_SERVER_PORT);
		InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket), m_iocp, m_serverKey, 0);

		if (WSAConnect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr), 0, 0, 0, 0) == SOCKET_ERROR)
		{
			ErrorQuit(L"Connection Failed");
		}

		// GQCS를 비동기로 돌릴 네트워크 스레드 생성
		m_networkThread = std::thread{ &NetworkManager::ProcessThread, this };
	}

	void NetworkManager::EndThreadProcess()
	{
		// 스레드 종료를 위한 QUIT 명령을 PQCS로 작업 예약
		OVERLAPPEDEX overEx{ network::COMPLETION::QUIT };

		PostQueuedCompletionStatus(m_iocp, 1, m_serverKey, &overEx.over);
	}

	void NetworkManager::CloseThread()
	{
		m_networkThread.join();

		closesocket(m_socket);
		WSACleanup();
	}

	void NetworkManager::Recv()
	{
		DWORD flag{};
		ZeroMemory(&m_recvEx.over, sizeof(m_recvEx.over));

		m_recvEx.wsa.len = network::CPacket::BUFF_SIZE + m_remainSize;
		m_recvEx.wsa.buf = m_recvEx.data + m_remainSize;

		WSARecv(m_socket, &m_recvEx.wsa, 1, 0, &flag, &m_recvEx.over, nullptr);
	}

	void NetworkManager::Send(network::CPacket& packet)
	{
		m_sendEx.Set(packet);

		WSASend(m_socket, &m_sendEx.wsa, 1, 0, 0, &m_sendEx.over, nullptr);
	}

	void NetworkManager::SendLoginPacket()
	{
		network::CPacket packet;
		auto pos{ GET_PLAYER[0]->GetTransform()->GetLocalPosition() };
		auto quat{ GET_PLAYER[0]->GetTransform()->GetLocalRotation() };
		auto scale{ GET_PLAYER[0]->GetTransform()->GetLocalScale() };

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_REQ);
		// 애니메이션 인덱스 작성
		//packet.Write<int32_t>(0);

		packet.WriteWString(GET_PLAYER[0]->GetName());

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.z);
		packet.Write<float>(1.f);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		// 패킷 전송
		Send(packet);
		Recv();
	}
#pragma endregion

#pragma region [PRIVATE]
	void NetworkManager::ProcessThread()
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

	void NetworkManager::Recv(DWORD bytes, network::OVERLAPPEDEX* pOverEx)
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
			packetSize = m_packet.GetPacketSize();

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

	void NetworkManager::Send(DWORD bytes, network::OVERLAPPEDEX* pOverEx)
	{
		ZeroMemory(&pOverEx->over, sizeof(pOverEx->over));
		pOverEx = nullptr;
		//delete pOverEx;
	}
#pragma endregion

#pragma region [PROCESS PACKET]
	void NetworkManager::ProcessPacket()
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

	void NetworkManager::ProcessAUPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_ACK:
			{
				m_id = m_packet.ReadID();

				AddPlayer(m_id);

				std::cout << "ADD ME" << std::endl << std::endl;
			}
			break;
			default:
			break;
		}
	}

	void NetworkManager::ProcessMYPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::MY_TRANSFORM_ACK:
			case ProtocolID::MY_JUMP_ACK:
			{
				TransformPlayer(m_id);
			}
			break;
			case ProtocolID::MY_ANI_ACK:
			{
				PlayAni(m_id);
			}
			break;
			default:
			break;
		}
	}

	void NetworkManager::ProcessWRPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::WR_ADD_ACK:
			{
				int32_t id{ m_packet.ReadID() };
				AddPlayer(id);

				std::cout << "ADD REMOTE" << std::endl << std::endl;
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				RemovePlayer(id);
			}
			break;
			case ProtocolID::WR_TRANSFORM_ACK:
			case ProtocolID::WR_JUMP_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				TransformPlayer(id);
			}
			break;
			case ProtocolID::WR_ANI_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				PlayAni(id);
			}
			break;
			default:
			break;
		}
	}

#pragma region [UNUSED]
	void NetworkManager::ProcessBTPacket(ProtocolID type)
	{
		// TODO : 추후 전투 관련 패킷 프로세스 처리
	}

	void NetworkManager::ProcessIFPacket(ProtocolID type)
	{
	}

	void NetworkManager::ProcessITPacket(ProtocolID type)
	{
		// TODO : 추후 아이템 관련 패킷 프로세스 처리
	}

	void NetworkManager::ProcessCMPacket(ProtocolID type)
	{
		// TODO : 추후 커뮤니티(eg. 채팅) 관련 패킷 프로세스 처리
	}

	void NetworkManager::ProcessECPacket(ProtocolID type)
	{
	}

	void NetworkManager::ProcessGMPacket(ProtocolID type)
	{
	}

	void NetworkManager::ProcessTTPacket(ProtocolID type)
	{
		// TODO : 추후 테스트 관련 패킷 프로세스 처리
	}
#pragma endregion

	void NetworkManager::AddPlayer(int32_t id)
	{
		std::wstring fbxName{};

		m_packet.ReadWString(fbxName);

		Vec3 pos{};
		pos.x = m_packet.Read<float>();
		pos.y = m_packet.Read<float>();
		pos.z = m_packet.Read<float>();

		Vec4 quat{};
		quat.x = m_packet.Read<float>();
		quat.y = m_packet.Read<float>();
		quat.z = m_packet.Read<float>();
		quat.w = m_packet.Read<float>();

		Vec3 scale{};
		scale.x = m_packet.Read<float>();
		scale.y = m_packet.Read<float>();
		scale.z = m_packet.Read<float>();

		std::cout << std::format("ID : {}\n", id);
		std::wcout << std::format(L"name : {}\n", fbxName);
		std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
		std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
		std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

		ObjectDesc objectDesc;
		objectDesc.strName = fbxName;
		objectDesc.strPath = L"..\\Resources\\FBX\\Moon\\" + fbxName + L".fbx";
		//objectDesc.strPath = L"..\\Resources\\FBX\\Dragon\\Dragon.fbx";
		objectDesc.vPostion = pos;
		objectDesc.vScale = scale;
		objectDesc.script = nullptr;// std::make_shared<Monster_Dragon>();

		std::shared_ptr<MeshData> meshData{ GET_SINGLE(Resources)->LoadFBX(objectDesc.strPath) };
		Object gameObjects{ meshData->Instantiate() };
		std::shared_ptr<network::CNetwork> networkComponent{ std::make_shared<network::CNetwork>() };

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(objectDesc.strName);
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(objectDesc.vPostion);
			gameObject->GetTransform()->SetLocalScale(objectDesc.vScale);
			//gameObject->AddComponent(objectDesc.script);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);
			gameObject->AddComponent(networkComponent);
		}

		GET_SCENE->AddPlayer(gameObjects);

		m_objects.insert(std::make_pair(id, gameObjects));

		for (auto& object : m_objects[id])
		{
			object->GetNetwork()->SetID(id);
		}
	}

	void NetworkManager::RemovePlayer(int32_t id)
	{
		GET_SCENE->RemovePlayer(m_objects[id]);

		m_objects.erase(id);
	}

	void NetworkManager::TransformPlayer(int32_t id)
	{
		// 패킷에서 오브젝트를 렌더링할 좌표 읽기
		Vec3 pos;
		pos.x = m_packet.Read<float>();
		pos.y = m_packet.Read<float>();
		pos.z = m_packet.Read<float>();

		Vec4 quat;
		quat.x = m_packet.Read<float>();
		quat.y = m_packet.Read<float>();
		quat.z = m_packet.Read<float>();
		quat.w = m_packet.Read<float>();

		// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
		for (auto& object : m_objects[id])
		{
			object->GetTransform()->SetLocalPosition(pos);
			// w좌표 수정 필요
			//object->GetTransform()->SetLocalRotation(quat);
		}
	}

	void NetworkManager::PlayAni(int32_t id)
	{
		int32_t aniIndex{ m_packet.Read<int32_t>() };
		float aniFrame{ m_packet.Read<float>() };

		for (auto& object : m_objects[id])
		{
			object->GetAnimator()->Play(aniIndex, aniFrame);
		}
	}
#pragma endregion
}