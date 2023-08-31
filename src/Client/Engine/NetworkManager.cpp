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
		m_alreadyAdded = false;

		Connect();
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

	void NetworkManager::SendLoginPacket(const std::wstring& ID, const std::wstring& pwd)
	{
		CPacket packet;

		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_REQ);
		//packet.WriteWString(ID);
		//packet.WriteWString(pwd);

		// 패킷 전송
		Send(packet);
		Recv();
	}

	void NetworkManager::SendKeyInputPacket()
	{
		CPacket packet;
		auto input{ GET_SINGLE(CInput)->GetKeyInput() };

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

	void NetworkManager::AddNetworkComponent(NetworkGameObject& object)
	{
		m_objects[m_id] = object;
	}

	void NetworkManager::AddNetworkObject(int32_t id, NetworkGameObject& object)
	{
		m_objects[id] = object;
	}

	void NetworkManager::RemoveNetworkObject(int32_t id)
	{
		m_objects.erase(id);
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
		m_packet.PushData(pOverEx->data);
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
			m_packet.PushData(m_pRecvPacket);
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
				m_login = true;
				m_id = m_packet.ReadID();
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
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				if (id == m_id)
				{
					if (m_alreadyAdded == true)
						break;

					for (auto& obj : m_objects[id])
					{
						obj->GetNetwork()->InsertPackets(m_packet);
					}

					m_alreadyAdded = true;
				}
				else
				{
					GET_SCENE->PushServerRequest(m_packet);
				}
			}
			break;
			case ProtocolID::WR_ADD_OBJ_ACK:
			case ProtocolID::WR_RENDER_EFFECT_ACK:
			case ProtocolID::WR_CHANGE_SOUND_ACK:
			case ProtocolID::WR_TRIGGER_INTERACTION_ACK:
			case ProtocolID::WR_TRIGGER_INTERACTION_COUNT_ACK:
			case ProtocolID::WR_PLAY_CUT_SCENE_ACK:
			{
				GET_SCENE->PushServerRequest(m_packet);
			}
			break;
			case ProtocolID::WR_REMOVE_ACK:
			{
				int32_t id{ m_packet.ReadID() };

				if (id == m_id)
					RemovePlayer(id);
				else
					GET_SCENE->PushServerRequest(m_packet);
			}
			break;
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

	void NetworkManager::RemovePlayer(int32_t id)
	{
		GET_SCENE->RemovePlayer(m_objects[id]);

		m_objects.erase(id);
	}
#pragma endregion
}