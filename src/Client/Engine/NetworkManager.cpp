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
#include "Input.h"
#include "MonoBehaviour.h"

namespace network
{
#pragma region [PUBLIC]
	void NetworkManager::Init(const std::wstring& serverAddr)
	{
		m_iocp = INVALID_HANDLE_VALUE;
		m_socket = INVALID_SOCKET;
		m_serverKey = 99999;
		m_pRecvPacket = nullptr;
		m_remainSize = 0;
		m_id = 0;
		m_login = false;
		m_serverAddr = serverAddr;

		Connect();
	}

	void NetworkManager::RegisterObject(server::OBJECT_TYPE type, std::shared_ptr<CGameObject> object)
	{
		// 해당 오브젝트가 기존에 존재하는 오브젝트인지 검사
		// 초기값인 -1이 아니라면 등록됬다는 뜻
		if (object->GetNetwork()->GetID() != -1)
		{
			// 존재하면 넘어감
			MSG_BOX(TEXT("Failed : NetworkManager::RegisterObject - Already exist Object"));
			return;
		}

		NetworkGameObject obj{ object };
		NetworkComponent component{ type, obj };
		m_unregisterdObjects.push_back(component);
	}

	void NetworkManager::RegisterObject(server::OBJECT_TYPE type, NetworkGameObject object)
	{
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
		}

		NetworkComponent component{ type, object };
		m_unregisterdObjects.push_back(component);
	}

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
		InetPton(AF_INET, m_serverAddr.c_str(), &serverAddr.sin_addr);

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
		OVERLAPPEDEX overEx{ COMPLETION::QUIT };

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

		m_recvEx.wsa.len = CPacket::BUFF_SIZE + m_remainSize;
		m_recvEx.wsa.buf = m_recvEx.data + m_remainSize;

		WSARecv(m_socket, &m_recvEx.wsa, 1, 0, &flag, &m_recvEx.over, nullptr);
	}

	void NetworkManager::Send(CPacket& packet)
	{
		m_sendEx.Set(packet);

		WSASend(m_socket, &m_sendEx.wsa, 1, 0, 0, &m_sendEx.over, nullptr);
	}

	void NetworkManager::SendLoginPacket()
	{
		CPacket packet;

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_REQ);

		// 패킷 전송
		Send(packet);
		Recv();
	}

	void NetworkManager::SendKeyInputPacket()
	{
		CPacket packet;
		auto input{ GET_SINGLE(Input)->GetKeyInput() };

		packet.WriteProtocol(ProtocolID::MY_KEYINPUT_REQ);
		packet.Write<unsigned long>(input);

		Send(packet);
	}

	void NetworkManager::SendLogoutPacket()
	{
		CPacket packet;

		packet.WriteProtocol(ProtocolID::AU_LOGOUT_REQ);

		Send(packet);
	}

	void NetworkManager::SendIDIssueRequest()
	{
		for (auto& component : m_unregisterdObjects)
		{
			ProtocolID protocol{ ProtocolID::PROTOCOL_NONE };

			if (component.type != server::OBJECT_TYPE::PLAYER)
				continue;

			/*switch (component.type)
			{
				case OBJECT_TYPE::PLAYER:
				{
					protocol = ProtocolID::MY_ISSUE_PLAYER_ID_REQ;
				}
				break;
				case OBJECT_TYPE::REMOTE_PLAYER:
				{
					protocol = ProtocolID::WR_ISSUE_PLAYER_ID_REQ;
				}
				break;
				case OBJECT_TYPE::OBJECT:
				{
					protocol = ProtocolID::WR_ISSUE_OBJ_ID_REQ;
				}
				break;
				default:
				break;
			}*/

			CPacket packet;
			packet.WriteProtocol(ProtocolID::MY_ISSUE_PLAYER_ID_REQ);
			Send(packet);
		}
	}

	void NetworkManager::AddNetworkObject(int32_t id, NetworkGameObject& object)
	{
		m_objects[id] = object;
	}

	void NetworkManager::ExchangeObjectID(int32_t oldID, int32_t newID)
	{
		auto obj{ m_objects.extract(oldID) };
		obj.key() = newID;

		m_objects.insert(std::move(obj));

		for (auto& object : m_objects[newID])
		{
			object->GetNetwork()->SetID(newID);
		}
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
					std::cout << "logout" << std::endl;
					SendLogoutPacket();
					return;
				}
				default:
				break;
			}
		}
	}

	void NetworkManager::Recv(DWORD bytes, OVERLAPPEDEX* pOverEx)
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

	void NetworkManager::Send(DWORD bytes, OVERLAPPEDEX* pOverEx)
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
				/*m_id = m_packet.ReadID();

				m_objects.insert(std::make_pair(m_id, GET_PLAYER));

				for (auto& object : m_objects[m_id])
				{
					object->GetNetwork()->SetID(m_id);
				}

				AddPlayer(m_id);

				std::cout << "ADD ME[" << m_id << "]" << std::endl << std::endl;*/
				m_login = true;
				std::cout << std::format("Login Success!") << std::endl;
			}
			break;
			case ProtocolID::AU_LOGOUT_ACK:
			{

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
			case ProtocolID::MY_ISSUE_PLAYER_ID_ACK:
			{
				m_id = m_packet.ReadID();

				std::list<NetworkComponent>::iterator iter;
				for (iter = m_unregisterdObjects.begin(); iter != m_unregisterdObjects.end(); ++iter)
				{
					if (iter->type == server::OBJECT_TYPE::PLAYER)
						break;
				}

				if (iter != m_unregisterdObjects.end())
				{
					m_objects[m_id] = iter->object;

					for (auto& obj : m_objects[m_id])
					{
						obj->GetNetwork()->SetID(m_id);
					}

					m_unregisterdObjects.erase(iter);

					std::cout << std::format("My id is {}\n", m_id);
				}
			}
			break;
			case ProtocolID::MY_ADD_OBJ_ACK:
			case ProtocolID::MY_ADD_OBJ_COLLIDER_ACK:
			{
				GET_SCENE->PushServerRequest(m_packet);
			}
			break;
			default:
			{
				for (auto& obj : m_objects[m_id])
				{
					obj->GetNetwork()->InsertPackets(m_packet);
				}
			}
			break;
		}
	}

	void NetworkManager::ProcessWRPacket(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::WR_ISSUE_PLAYER_ID_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				/*std::list<NetworkComponent>::iterator iter;
				for (iter = m_unregisterdObjects.begin(); iter != m_unregisterdObjects.end(); ++iter)
				{
					if (iter->type == OBJECT_TYPE::REMOTE_PLAYER)
						break;
				}

				if (iter != m_unregisterdObjects.end())
				{
					m_objects[id] = iter->object;

					for (auto& obj : m_objects[id])
					{
						obj->GetNetwork()->SetID(id);
					}

					m_unregisterdObjects.erase(iter);

					std::cout << std::format("Remote id is {}\n", id);
				}*/
			}
			break;
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				if (id == m_id)
				{
					for (auto& obj : m_objects[id])
					{
						obj->GetNetwork()->InsertPackets(m_packet);
					}
				}
				else
				{
					GET_SCENE->PushServerRequest(m_packet);
				}
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				RemovePlayer(id);
			}
			break;
			/*case ProtocolID::WR_TRANSFORM_ACK:
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
			break;*/
			default:
			{
				int32_t id{ m_packet.ReadID() };

				for (auto& obj : m_objects[id])
				{
					obj->GetNetwork()->InsertPackets(m_packet);
				}
			}
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

		int32_t aniIndex{ m_packet.Read<int32_t>() };
		float aniFrame{ m_packet.Read<float>() };

		std::wstring newName{ fbxName + std::to_wstring(id) };

		std::cout << std::format("ID : {}\n", id);
		std::wcout << std::format(L"name : {}\n", newName);
		std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
		std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
		std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

		ObjectDesc objectDesc;
		objectDesc.strName = newName;
		objectDesc.strPath = L"..\\Resources\\FBX\\Character\\" + fbxName + L"\\" + fbxName + L".fbx";
		objectDesc.vPostion = pos;	// pos;
		objectDesc.vScale = scale;	// scale;
		objectDesc.script = nullptr;

		std::shared_ptr<MeshData> meshData{ GET_SINGLE(Resources)->LoadFBX(objectDesc.strPath) };
		std::vector<std::shared_ptr<CGameObject>> gameObjects{ meshData->Instantiate() };
		std::shared_ptr<CNetwork> networkComponent{ std::make_shared<CNetwork>(server::OBJECT_TYPE::PLAYER, id) };

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(objectDesc.strName);
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(objectDesc.vPostion);
			gameObject->GetTransform()->SetLocalScale(objectDesc.vScale);
			gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			gameObject->AddComponent(objectDesc.script);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			gameObject->AddComponent(networkComponent);
			gameObject->GetAnimator()->Play(aniIndex, aniFrame);
		}

		GET_SCENE->AddPlayer(gameObjects);

		m_objects[id] = gameObjects;
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

		Vec3 scale;
		scale.x = m_packet.Read<float>();
		scale.y = m_packet.Read<float>();
		scale.z = m_packet.Read<float>();

		// 오브젝트를 렌더링할 좌표를 오브젝트에 설정
		for (auto& object : m_objects[id])
		{
			auto transform = object->GetTransform();

			transform->SetWorldVec3Position(pos);
			auto mat{ Matrix::CreateTranslation(pos) };
			transform->SetWorldMatrix(mat);
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

		std::cout << "changed player[" << id << "] animation to " << aniIndex << std::endl;
	}
#pragma endregion
}