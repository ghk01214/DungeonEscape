#include "pch.h"
#include "Session.h"
#include "Transform.h"

namespace game
{
	CSession::CSession() :
		m_recvEx{},
		m_sendEx{},
		m_state{ STATE::FREE },
		m_socket{ INVALID_SOCKET },
		m_id{ -1 },
		m_pObject{ nullptr },
		m_prevRemain{ 0 }
	{
	}

	CSession::CSession(Player* obj) :
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

	void CSession::SendLoginPacket(Player* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };

		// 패킷을 전송할 클라이언트 id 작성
		packet.WriteID(m_id);
		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::AU_LOGIN_ACK);

		packet.WriteWString(obj->GetName());

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);
		packet.Write<float>(quat.z);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		packet.Write<server::SCRIPT_TYPE>(obj->GetScriptType());
		packet.Write<int32_t>(obj->GetAniIndex());
		packet.Write<float>(obj->GetAniFrame());

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendAddAnimateObjPacket(int32_t id, Player* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };
		auto scriptType{ obj->GetScriptType() };

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_ADD_ANIMATE_OBJ_ACK);

		packet.WriteWString(obj->GetName());

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);
		packet.Write<float>(quat.z);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		packet.Write<server::SCRIPT_TYPE>(obj->GetScriptType());
		packet.Write<int32_t>(obj->GetAniIndex());
		packet.Write<float>(obj->GetAniFrame());

		Send(packet);
	}

	void CSession::SendAddObjPacket(int32_t id, GameObject* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_ADD_OBJ_ACK);

		packet.WriteWString(obj->GetName());

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);
		packet.Write<float>(quat.z);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		Send(packet);
	}

	void CSession::SendRemovePacket(int32_t id)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_REMOVE_ACK);

		Send(packet);
	}

	void CSession::SendTransformPacket(int32_t id, ProtocolID protocol, GameObject* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };

		// 오른손 > 왼손
		pos.z = -pos.z;
		quat.z = -quat.z;
		quat.Normalize();

		// 타깃 id 작성
		packet.WriteID(id);
		// 프로토콜 종류 작성
		packet.WriteProtocol(protocol);

		// 타깃 렌더링 좌표 작성
		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);
		packet.Write<float>(quat.z);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendAniIndexPacket(int32_t id, ProtocolID protocol, Player* obj)
	{
		network::CPacket packet;
		int32_t aniIndex{ obj->GetAniIndex() };
		float aniFrame{ obj->GetAniFrame() };

		packet.WriteID(id);
		packet.WriteProtocol(protocol);
		packet.Write<int32_t>(aniIndex);
		packet.Write<float>(aniFrame);

		Send(packet);
	}

	void CSession::SendAniIndexPacket(int32_t id, ProtocolID protocol, int32_t index, float frame)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(protocol);
		packet.Write<int32_t>(index);
		packet.Write<float>(frame);

		Send(packet);
	}
}