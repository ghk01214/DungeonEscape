#include "pch.h"
#include "Session.h"
#include "Transform.h"
#include "Monsters.hpp"
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

		packet.WriteID(m_id);
		packet.WriteProtocol(ProtocolID::AU_LOGIN_ACK);

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

	void CSession::SendAddAnimateObjPacket(GameObject* obj, float scaleRatio)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto scale{ obj->GetTransform()->GetScale() };
		auto objType{ obj->GetObjectType() };
		auto fbxType{ obj->GetFBXType() };

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::WR_ADD_ANIMATE_OBJ_ACK);

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		packet.Write<float>(scaleRatio);

		switch (fbxType)
		{
			case server::FBX_TYPE::NANA:
			case server::FBX_TYPE::MISTIC:
			case server::FBX_TYPE::CARMEL:
			{
				auto player{ dynamic_cast<Player*>(obj) };

				packet.Write<int32_t>(player->GetState());
				packet.Write<float>(player->GetAniPlayTime());
			}
			break;
			case server::FBX_TYPE::WEEPER1:
			case server::FBX_TYPE::WEEPER2:
			case server::FBX_TYPE::WEEPER3:
			case server::FBX_TYPE::WEEPER4:
			case server::FBX_TYPE::WEEPER5:
			case server::FBX_TYPE::WEEPER6:
			case server::FBX_TYPE::WEEPER7:
			case server::FBX_TYPE::WEEPER_EMISSIVE:
			{
				auto monster{ dynamic_cast<Weeper*>(obj) };

				packet.Write<int32_t>(monster->GetState());
				packet.Write<float>(monster->GetAniPlayTime());
			}
			break;
			case server::FBX_TYPE::BLUE_GOLEM:
			case server::FBX_TYPE::GREEN_GOLEM:
			case server::FBX_TYPE::RED_GOLEM:
			{
				auto monster{ dynamic_cast<Golem*>(obj) };

				packet.Write<int32_t>(monster->GetState());
				packet.Write<float>(monster->GetAniPlayTime());
			}
			break;
			case server::FBX_TYPE::DRAGON:
			{
				auto monster{ dynamic_cast<Dragon*>(obj) };

				packet.Write<int32_t>(monster->GetState());
				packet.Write<float>(monster->GetAniPlayTime());
			}
			break;
			default:
			break;
		}

		packet.Write<server::OBJECT_TYPE>(objType);
		packet.Write<server::FBX_TYPE>(fbxType);

		Send(packet);
	}

	void CSession::SendAddObjPacket(GameObject* obj, float scaleRatio)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::WR_ADD_OBJ_ACK);

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.z);
		packet.Write<float>(quat.w);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		packet.Write<float>(scaleRatio);

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

	void CSession::SendRemovePacket(int32_t id, server::OBJECT_TYPE type)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_REMOVE_ACK);
		packet.Write<server::OBJECT_TYPE>(type);

		Send(packet);
	}

	void CSession::SendPlayerTransformPacket(GameObject* obj)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };

		// 타깃 id 작성
		packet.WriteID(obj->GetID());
		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::WR_TRANSFORM_ACK);

		// 타깃 렌더링 좌표 작성
		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendTransformPacket(GameObject* obj, float scaleRatio)
	{
		network::CPacket packet;
		Transform* trans{ obj->GetTransform() };
		auto pos{ trans->GetPosition() };
		auto quat{ trans->GetRotation() };
		auto scale{ trans->GetScale() };

		// 타깃 id 작성
		packet.WriteID(obj->GetID());
		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::WR_TRANSFORM_ACK);

		// 오른손 > 왼손
		/*pos.z = -pos.z;
		quat.z = -quat.z;*/
		quat.Normalize();

		// 타깃 렌더링 좌표 작성
		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.z);
		packet.Write<float>(quat.w);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		packet.Write<float>(scaleRatio);

		// 패킷 전송
		Send(packet);
	}

	void CSession::SendStatePacket(int32_t id, int32_t stateIndex)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_CHANGE_STATE_ACK);
		packet.Write<int32_t>(stateIndex);

		Send(packet);
	}

	void CSession::SendMonsterQuaternionPacket(Monster* obj)
	{
		network::CPacket packet;
		auto rotation{ obj->GetRotation() };

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::WR_MONSTER_QUAT_ACK);
		packet.Write<float>(rotation.y);
		packet.Write<float>(rotation.w);

		Send(packet);
	}

	void CSession::SendMonsterPatternPacket(int32_t id, int32_t patternIndex)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_MONSTER_PATTERN_ACK);
		packet.Write<int32_t>(patternIndex);

		Send(packet);
	}

	void CSession::SendSkillHitPacket(int32_t id)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_SKILL_HIT_ACK);

		Send(packet);
	}

	void CSession::SendPlayerHPPacket(Player* obj)
	{
		network::CPacket packet;

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::WR_PLAYER_HP_ACK);
		packet.Write<int32_t>(obj->GetHP());

		Send(packet);
	}

	void CSession::SendPlayerMPPacket(Player* obj)
	{
		network::CPacket packet;

		packet.WriteID(obj->GetID());
		packet.WriteProtocol(ProtocolID::WR_PLAYER_MP_ACK);
		packet.Write<int32_t>(obj->GetMP());

		Send(packet);
	}

	void CSession::SendRenderEffectPacket(int32_t id, int32_t effectIndex, Vec3 effectPos)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_RENDER_EFFECT_ACK);
		packet.Write<int32_t>(effectIndex);

		packet.Write<float>(effectPos.x);
		packet.Write<float>(effectPos.y);
		packet.Write<float>(effectPos.z);

		Send(packet);
	}

	void CSession::SendChangeSoundPacket(server::SOUND_TYPE soundType)
	{
		network::CPacket packet;

		packet.WriteID(m_id);
		packet.WriteProtocol(ProtocolID::WR_CHANGE_SOUND_ACK);
		packet.Write<server::SOUND_TYPE>(soundType);

		Send(packet);
	}

	void CSession::SendTriggerInteractionPacket(int32_t id, server::TRIGGER_INTERACTION_TYPE type)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_TRIGGER_INTERACTION_ACK);
		packet.Write<server::TRIGGER_INTERACTION_TYPE>(type);

		Send(packet);
	}

	void CSession::SendTriggerInteractionCountPacket(int32_t id, server::TRIGGER_INTERACTION_TYPE type, int32_t size)
	{
		network::CPacket packet;

		packet.WriteID(id);
		packet.WriteProtocol(ProtocolID::WR_TRIGGER_INTERACTION_COUNT_ACK);
		packet.Write<server::TRIGGER_INTERACTION_TYPE>(type);
		packet.Write<int32_t>(size);

		Send(packet);
	}
}