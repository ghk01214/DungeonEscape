#include "pch.h"
#include "Network.h"

#include "Transform.h"
#include "Resources.h"
#include "MeshData.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Timer.h"

#include "NetworkManager.h"

namespace network
{
	CNetwork::CNetwork(int32_t id) :
		Component{ COMPONENT_TYPE::NETWORK },
		m_networkID{ id }
	{
	}

	CNetwork::~CNetwork()
	{
	}

	void CNetwork::Awake()
	{
		// 네트워크 매니저에 자신을 가지고 있는 게임 오브젝트 정보를 넘기면, objectID를 세팅해줌
		//GET_NETWORK->RegisterObject(m_gameObject.lock());
	}

	void CNetwork::FinalUpdate()
	{
	}

#pragma region [SEND PACKET]
	void CNetwork::SendAddObjectPacket()
	{
		CPacket packet;
		auto pos{ GetTransform()->GetLocalPosition() };
		auto quat{ GetTransform()->GetLocalRotation() };
		auto scale{ GetTransform()->GetLocalScale() };

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_ADD_OBJ_REQ);

		packet.WriteWString(GetName());

		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		packet.Write<float>(quat.x);
		packet.Write<float>(quat.y);
		packet.Write<float>(quat.z);
		packet.Write<float>(1.f);

		packet.Write<float>(scale.x);
		packet.Write<float>(scale.y);
		packet.Write<float>(scale.z);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAniIndexPacket(int32_t index, float updateTime)
	{
		CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		packet.Write<int32_t>(index);
		packet.Write<float>(updateTime);

		GET_NETWORK->Send(packet);
	}
#pragma endregion
}