#include "pch.h"
#include <OVERLAPPEDEX.h>
#include "Object.h"
#include "Session.h"

using namespace std::chrono;

namespace game
{
	CSession::CSession() :
		m_recvEx{},
		m_sendEx{},
		m_state{ STATE::FREE },
		m_socket{ INVALID_SOCKET },
		m_id{ -1 },
		m_pObject{ nullptr },
		m_prevRemain{ 0 },
		m_targetNum{ 0 }
	{
	}

	CSession::CSession(CObject* obj) :
		m_recvEx{},
		m_sendEx{},
		m_state{ STATE::FREE },
		m_socket{ INVALID_SOCKET },
		m_id{ -1 },
		m_pObject{ obj },
		m_prevRemain{ 0 }
	{
	}

	CSession::~CSession()
	{
		closesocket(m_socket);
	}

	void CSession::Reset()
	{
		closesocket(m_socket);

		m_state.store(STATE::FREE, std::memory_order_seq_cst);

		m_id = -1;
	}

	void CSession::Recv()
	{
		DWORD flag{ 0 };

		m_recvEx.wsa.len = network::CPacket::BUFF_SIZE - m_prevRemain;
		m_recvEx.wsa.buf = m_recvEx.data + m_prevRemain;

		ZeroMemory(&m_recvEx.over, sizeof(m_recvEx.over));
		WSARecv(m_socket, &m_recvEx.wsa, 1, 0, &flag, &m_recvEx.over, nullptr);
	}

	void CSession::Send(network::CPacket& packet)
	{
		// OVERLAPPEDEX에 패킷 데이터 복사
		m_sendEx.Set(packet);

		WSASend(m_socket, &m_sendEx.wsa, 1, 0, 0, &m_sendEx.over, nullptr);
	}

	void CSession::SendLoginPacket()
	{
		network::CPacket packet;

		// 패킷을 전송할 클라이언트 id 작성
		packet.WriteID(m_id);
		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::AU_LOGIN_ACK);
		// 클라이언트의 메인 타깃 인덱스 작성
		packet.Write<uint16_t>(m_targetNum);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendMovePacket(int32_t id, uint16_t targetNum, CObject* obj)
	{
		network::CPacket packet;
		Pos pos{ obj->GetPos() };

		// 패킷을 전송할 클라이언트 id 작성
		packet.WriteID(id);
		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::MY_MOVE_ACK);
		// 이동할 타깃 인덱스 작성
		packet.Write<uint16_t>(targetNum);
		// 타깃 렌더링 좌표 작성
		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendRotatePacket(int32_t id, uint16_t targetNum, CObject* obj)
	{
		network::CPacket packet;
		Rotation rotate{ obj->GetRotation() };

		// 패킷을 전송할 클라이언트 id 작성
		packet.WriteID(id);
		// 프로토콜 종류 작성
		packet.Write<ProtocolID>(ProtocolID::MY_MOVE_ACK);
		// 이동할 타깃 인덱스 작성
		packet.Write<uint16_t>(targetNum);
		// 타깃 렌더링 회전각 작성
		packet.Write<float>(rotate.x);
		packet.Write<float>(rotate.y);
		packet.Write<float>(rotate.z);

		// 패킷 전송
		Send(packet);
	}
}