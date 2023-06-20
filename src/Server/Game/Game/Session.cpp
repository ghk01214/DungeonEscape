﻿#include "pch.h"
#include "Session.h"
#include "Transform.h"
#include "Monster.h"
#include "CustomController.h"

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
		//Transform* trans{ obj->GetTransform() };
		//auto pos{ trans->GetPosition() };
		//auto quat{ trans->GetRotation() };
		//auto scale{ trans->GetScale() };

		packet.WriteID(m_id);
		packet.WriteProtocol(ProtocolID::AU_LOGIN_ACK);

		//packet.WriteWString(obj->GetName());

		//packet.Write<float>(pos.x);
		//packet.Write<float>(pos.y);
		//packet.Write<float>(pos.z);

		//packet.Write<float>(quat.x);
		//packet.Write<float>(quat.y);
		//packet.Write<float>(quat.w);
		//packet.Write<float>(quat.z);

		//packet.Write<float>(scale.x);
		//packet.Write<float>(scale.y);
		//packet.Write<float>(scale.z);

		//packet.Write<int32_t>(obj->GetAniIndex());
		//packet.Write<float>(obj->GetAniFrame());

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendPlayerIDIssuePacket(int32_t id, ProtocolID protocol)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(protocol);

		Send(packet);
	}

	void CSession::SendIDIssuePacket(GameObject* obj)
	{
		network::CPacket packet;

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::MY_ISSUE_PLAYER_ID_ACK);

		Send(packet);
	}

	void CSession::SendAddAnimateObjPacket(int32_t id, GameObject* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };
		auto objType{ obj->GetObjectType() };

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_ADD_ANIMATE_OBJ_ACK);

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

		if (objType == server::OBJECT_TYPE::PLAYER)
		{
			auto player{ dynamic_cast<Player*>(obj) };

			packet.Write<int32_t>(player->GetAniIndex());
			packet.Write<float>(player->GetAniFrame());
		}
		else if (objType == server::OBJECT_TYPE::BOSS)
		{
			auto monster{ dynamic_cast<Monster*>(obj) };

			packet.Write<int32_t>(monster->GetAniIndex());
			packet.Write<float>(monster->GetAniFrame());
		}

		packet.Write<server::OBJECT_TYPE>(objType);
		packet.Write<server::FBX_TYPE>(obj->GetFBXType());

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

		packet.Write<server::OBJECT_TYPE>(obj->GetObjectType());
		packet.Write<server::FBX_TYPE>(obj->GetFBXType());

		Send(packet);
	}

	void CSession::SendObjectIDPacket(int32_t objID, int32_t oldObjID)
	{
		network::CPacket packet;

		packet.WriteID(objID);
		packet.WriteProtocol(ProtocolID::MY_ADD_OBJ_ACK);

		packet.Write<int32_t>(oldObjID);

		Send(packet);
	}

	void CSession::SendAddObjectColliderPacket(int32_t id, int32_t objID, int32_t colliderID, int32_t tempColliderID, bool last)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::MY_ADD_OBJ_COLLIDER_ACK);

		packet.Write<int32_t>(objID);
		packet.Write<int32_t>(colliderID);
		packet.Write<int32_t>(tempColliderID);
		packet.Write<bool>(last);

		Send(packet);
	}

	void CSession::SendRemovePacket(int32_t id, server::OBJECT_TYPE type)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_REMOVE_ACK);
		packet.Write<server::OBJECT_TYPE>(type);

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
		/*pos.z = -pos.z;
		quat.z = -quat.z;*/
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

		if (obj->GetObjectType() == server::OBJECT_TYPE::PLAYER)
		{
			auto player{ dynamic_cast<Player*>(obj) };
			packet.Write<bool>(player->IsOnGound());
		}
		else if (obj->GetObjectType() == server::OBJECT_TYPE::BOSS)
		{
			auto monster{ dynamic_cast<Monster*>(obj) };
			packet.Write<bool>(monster->IsOnGround());
		}

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendAniIndexPacket(int32_t id, ProtocolID protocol, Player* obj)
	{
		network::CPacket packet;
		int32_t aniIndex{ obj->GetAniIndex() };
		float aniFrame{ obj->GetAniFrame() };
		float aniSpeed{ obj->GetAniSpeed() };

		packet.WriteID(id);
		packet.WriteProtocol(protocol);
		packet.Write<int32_t>(aniIndex);
		packet.Write<float>(aniFrame);
		packet.Write<float>(aniSpeed);

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

	void CSession::SendJumpStartPacket(int32_t id)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_JUMP_START_ACK);

		Send(packet);
	}

	void CSession::SendHitPacket(int32_t id, int32_t hitOriginID)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_HIT_ACK);
		packet.Write<int32_t>(hitOriginID);

		Send(packet);
	}

	void CSession::SendDiePacket(int32_t id)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_DIE_ACK);

		Send(packet);
	}
}