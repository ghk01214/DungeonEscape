#include "pch.h"
#include "Network.h"
#include "NetworkManager.h"

CNetwork::CNetwork() : Component(COMPONENT_TYPE::NETWORK)
{
}

CNetwork::~CNetwork()
{
}

void CNetwork::Awake()
{
	// 네트워크 매니저에 자신을 가지고 있는 게임 오브젝트 정보를 넘기면, objectID를 세팅해줌
	GET_SINGLE(NetworkManager)->RegisterObject(m_gameObject.lock());
}

void CNetwork::FinalUpdate()
{
}
