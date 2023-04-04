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
	CNetwork::CNetwork() :
		Component{ COMPONENT_TYPE::NETWORK },
		m_networkID{ -1 }
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
	void CNetwork::SendTransformPacket(DIRECTION direction, ROTATION rotation)
	{
		network::CPacket packet;

		packet.WriteID(m_networkID);
		// 프로토콜 종류 작성
		packet.WriteProtocol(ProtocolID::MY_TRANSFORM_REQ);
		// 델타 타임 작성
		packet.Write<float>(DELTA_TIME);
		// 오브젝트 이동방향 작성
		packet.Write<DIRECTION>(direction);
		packet.Write<ROTATION>(rotation);

		// 패킷 전송
		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendJumpPacket()
	{
		network::CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_JUMP_REQ);
		packet.Write<float>(DELTA_TIME);

		GET_NETWORK->Send(packet);
	}

	void CNetwork::SendAniIndexPacket(int32_t index, float updateTime)
	{
		network::CPacket packet;

		packet.WriteID(m_networkID);
		packet.WriteProtocol(ProtocolID::MY_ANI_REQ);
		packet.Write<int32_t>(index);
		packet.Write<float>(updateTime);

		GET_NETWORK->Send(packet);
	}
#pragma endregion
}