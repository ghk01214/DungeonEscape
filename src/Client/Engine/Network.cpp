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

	std::deque<CPacket>& CNetwork::GetRecvPackets()
	{
		//if (m_myPlayer == true)
		//	std::cout << m_recvPackets.size() << ", ";
		return m_recvPackets;
	}

	const int32_t CNetwork::GetRecvQueueSize() const
	{
		return m_recvQueueSize;
	}

#pragma region [SEND PACKET]
	void CNetwork::SendAddPlayer(server::FBX_TYPE fbxType)
	{
		CPacket packet;

		packet.WriteProtocol(ProtocolID::MY_ADD_ANIMATE_OBJ_REQ);
		packet.Write<server::OBJECT_TYPE>(m_objectType);
		packet.Write<server::FBX_TYPE>(fbxType);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAniIndex(server::OBJECT_TYPE type)
	{
		CPacket packet;
		auto anim{ GetAnimator() };

		packet.WriteID(m_networkID);
		//packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		packet.Write<server::OBJECT_TYPE>(type);
		packet.Write<int32_t>(anim->GetCurrentClipIndex());
		packet.Write<float>(anim->GetUpdateTime());
		packet.Write<float>(anim->GetAnimSpeed());

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendPlayerQuat(const Quat& quat)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_PLAYER_LOOK_REQ);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.w);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAnimationEnd(server::OBJECT_TYPE type, int32_t state)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::WR_ANI_END_REQ);
		packet.Write<server::OBJECT_TYPE>(type);
		packet.Write<int32_t>(state);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendCameraLook(const Vec3& look)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
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

	void CNetwork::SendRemoveObject(server::OBJECT_TYPE type)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::WR_REMOVE_REQ);
		packet.Write<server::OBJECT_TYPE>(type);

		GET_NETWORK->Send(packet);
	}
#pragma endregion
}