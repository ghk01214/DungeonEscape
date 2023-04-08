#include "pch.h"
#include "Session.h"

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
		m_keyInput{ 0 },
		m_keyState{ server::KEY_STATE::NONE }
	{
	}

	CSession::CSession(CObject* obj) :
		m_recvEx{},
		m_sendEx{},
		m_state{ STATE::FREE },
		m_socket{ INVALID_SOCKET },
		m_id{ -1 },
		m_pObject{ obj },
		m_prevRemain{ 0 },
		m_keyInput{ 0 },
		m_keyState{ server::KEY_STATE::NONE }
	{
	}

	CSession::~CSession()
	{
		closesocket(m_socket);
	}

	void CSession::Reset()
	{
		closesocket(m_socket);

		m_state = STATE::FREE;

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

	void CSession::SendLoginPacket(int32_t id, CObject* obj)
	{
		network::CPacket packet;
		Trans trans{ obj->GetTransform() };

		// 패킷을 전송할 클라이언트 id 작성
		packet.WriteID(id);
		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_ACK);

		packet.Write<float>(trans.p.x);
		packet.Write<float>(trans.p.y);
		packet.Write<float>(trans.p.z);

		packet.Write<float>(trans.q.x);
		packet.Write<float>(trans.q.y);
		packet.Write<float>(trans.q.z);
		packet.Write<float>(trans.q.w);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendAddPacket(int32_t id, CObject* obj)
	{
		network::CPacket packet;
		Trans trans{ obj->GetTransform() };

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_ADD_ACK);

		packet.Write<float>(trans.p.x);
		packet.Write<float>(trans.p.y);
		packet.Write<float>(trans.p.z);

		packet.Write<float>(trans.q.x);
		packet.Write<float>(trans.q.y);
		packet.Write<float>(trans.q.z);
		packet.Write<float>(trans.q.w);

		Send(packet);
	}

	void CSession::SendRemovePacket(int32_t id)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_REMOVE_ACK);

		Send(packet);
	}

	void CSession::SendTransformPacket(int32_t id, ProtocolID protocol, CObject* obj)
	{
		network::CPacket packet;
		Trans trans{ obj->GetTransform() };

		// 타깃 id 작성
		packet.WriteID(id);
		// 프로토콜 종류 작성
		packet.WriteProtocol(protocol);
		// 타깃 렌더링 좌표 작성
		packet.Write<float>(trans.p.x);
		packet.Write<float>(trans.p.y);
		packet.Write<float>(trans.p.z);

		packet.Write<float>(trans.q.x);
		packet.Write<float>(trans.q.y);
		packet.Write<float>(trans.q.z);
		packet.Write<float>(trans.q.w);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendAniIndexPacket(int32_t id, ProtocolID protocol, CPlayer* obj)
	{
		network::CPacket packet;
		int32_t index{ obj->GetAniIndex() };
		float frame{ obj->GetAniFrame() };

		packet.WriteID(id);
		packet.WriteProtocol(protocol);
		packet.Write<int32_t>(index);
		packet.Write<float>(frame);

		Send(packet);
	}
}