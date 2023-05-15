#include "pch.h"
#include "NetworkManager.h"
#include "Network.h"

#include "Transform.h"
#include "Resources.h"
#include "MeshData.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"
#include "Animator.h"

namespace network
{
	CNetwork::CNetwork(OBJECT_TYPE type, int32_t id) :
		Component{ COMPONENT_TYPE::NETWORK },
		m_networkID{ id },
		m_objectType{ type },
		m_recvQueueSize{ 0 },
		m_myPlayer{ false }
	{
		if (m_objectType == OBJECT_TYPE::PLAYER)
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
	void CNetwork::SendAddPlayer()
	{
		CPacket packet;

		packet.WriteProtocol(ProtocolID::MY_ADD_ANIMATE_OBJ_REQ);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAniIndexPacket(int32_t index)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		packet.Write<int32_t>(index);
		packet.Write<float>(GetAnimator()->GetUpdateTime());

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendCameraLook(const Vec3& look)
	{
		network::CPacket packet;

		packet.WriteID(GetNetwork()->GetID());
		packet.WriteProtocol(ProtocolID::MY_CAMERA_LOOK_REQ);

		packet.Write<float>(look.x);
		packet.Write<float>(look.y);
		packet.Write<float>(look.z);

		GET_NETWORK->Send(packet);
	}
#pragma endregion
}