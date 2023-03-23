#include "pch.h"
#include "NetworkManager.h"

#include "GameObject.h"
#include "Network.h"

void NetworkManager::Update()
{
}

void NetworkManager::RegiserObject(std::shared_ptr<CGameObject> object)
{
	// 해당 오브젝트가 기존에 존재하는 오브젝트인지 검사
	// 초기값인 -1이 아니라면 등록됬다는 뜻
	if (-1 != object->GetNetwork()->GetObjectID())
	{
		// 존재하면 넘어감
		MSG_BOX(TEXT("Failed : NetworkManager::RegiserObject - Already exist Object"));
		return;
	}

	// 0번부터 순서대로 ObjectID 부여
	uint32 objectID = m_networkObject.size();

	// 인자로 받은 게임 오브젝트의 네트워크 컴포넌트에 objectID 부여
	object->GetNetwork()->SetObjectID(objectID);

	// 멤버 관리 변수에 추가
	m_networkObject.emplace(objectID, object);
}