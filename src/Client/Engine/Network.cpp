#include "pch.h"
#include "NetworkManager.h"
#include "GameObject.h"
#include "Network.h"

#include "Resources.h"
#include "Transform.h"
#include "MeshData.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"
#include "Animator.h"

namespace network
{
	CNetwork::CNetwork(server::OBJECT_TYPE type, int32_t id) :
		Component{ COMPONENT_TYPE::NETWORK },
		m_networkID{ id },
		m_objectType{ type },
		m_recvQueueSize{ 0 },
		m_myPlayer{ false }
	{
		if (m_objectType == server::OBJECT_TYPE::PLAYER)
			m_myPlayer = true;
	}

	CNetwork::~CNetwork()
	{
	}

	void CNetwork::Awake()
	{
		// 네트워크 매니저에 자신을 가지고 있는 게임 오브젝트 정보를 넘기면, objectID를 세팅해줌
		GET_NETWORK->RegisterObject(m_objectType, m_gameObject.lock());
	}

	void CNetwork::FinalUpdate()
	{
	}

	void CNetwork::InsertPackets(CPacket& packet)
	{
		m_recvPackets.push_back(packet);
		++m_recvQueueSize;
	}

	void CNetwork::ClearRecvQueue(int32_t size)
	{
		for (int32_t i = 0; i < size; ++i)
		{
			if (m_recvPackets.empty() == true)
			{
				m_recvQueueSize = 0;
				return;
			}

			m_recvPackets.pop_front();
			--m_recvQueueSize;
		}
	}

	const std::deque<CPacket> CNetwork::GetRecvPackets() const
	{
		return m_recvPackets;
	}

	const int32_t CNetwork::GetRecvQueueSize() const
	{
		return m_recvQueueSize.load();
	}

#pragma region [SEND PACKET]
	void CNetwork::SendAddPlayer(server::FBX_TYPE fbxType)
	{
		CPacket packet;

		packet.WriteProtocol(ProtocolID::MY_ADD_ANIMATE_OBJ_REQ);
		packet.Write<server::FBX_TYPE>(fbxType);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAniIndexPacket()
	{
		CPacket packet;
		auto anim{ GetAnimator() };

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		packet.Write<int32_t>(anim->GetCurrentClipIndex());
		packet.Write<float>(anim->GetUpdateTime());
		packet.Write<float>(anim->GetAnimSpeed());

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendCameraLook(const Vec3& look)
	{
		CPacket packet;

		packet.WriteID(GetNetwork()->GetID());
		packet.WriteProtocol(ProtocolID::MY_CAMERA_LOOK_REQ);

		packet.Write<float>(look.x);
		packet.Write<float>(look.y);
		packet.Write<float>(look.z);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAddObject(int32_t tempID, server::OBJECT_TYPE objType, server::FBX_TYPE fbxType)
	{
		CPacket packet;

		packet.WriteID(tempID);
		packet.WriteProtocol(ProtocolID::MY_ADD_OBJ_REQ);

		packet.Write<server::OBJECT_TYPE>(objType);
		packet.Write<server::FBX_TYPE>(fbxType);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAddObjectCollider(int32_t tempID, int32_t tempColliderID, Collider& collider, bool last)
	{
		CPacket packet;

		packet.WriteID(tempID);
		packet.WriteProtocol(ProtocolID::MY_ADD_OBJ_COLLIDER_REQ);

		packet.Write<int32_t>(tempColliderID);
		packet.Write<server::COLLIDER_TYPE>(collider.type);

		packet.Write<float>(collider.pos.x);
		packet.Write<float>(collider.pos.y);
		packet.Write<float>(collider.pos.z);

		packet.Write<float>(collider.quat.x);
		packet.Write<float>(collider.quat.y);
		packet.Write<float>(collider.quat.z);
		packet.Write<float>(collider.quat.w);

		packet.Write<float>(collider.scale.x);
		packet.Write<float>(collider.scale.y);
		packet.Write<float>(collider.scale.z);

		packet.Write<bool>(last);

		GET_NETWORK->Send(packet);
	}
#pragma endregion
}