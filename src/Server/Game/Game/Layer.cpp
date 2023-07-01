#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

void Layer::Init()
{
    std::random_device rd;
    m_randomEngine = std::mt19937(rd());
}

void Layer::Update(double timeDelta)
{
	for (auto& gameObject : m_GameObjects)
	{
		gameObject->Update(timeDelta);
	}
}

void Layer::LateUpdate(double timeDelta)
{
    for (auto& gameObject : m_GameObjects)
    {
        // lateupdate진행
        gameObject->LateUpdate(timeDelta);
    }

    for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
    {
        // 객체 삭제 플래그값 확인
        if ((*it)->GetRemovalFlag())
        {
            // 오브젝트 삭제
            (*it)->Release();
            delete* it;

            // 메인 컨테이너에 삭제된 오브젝트들을 제외시킨다.
            it = m_GameObjects.erase(it);
        }
        else
        {
            // 삭제가 진행되지 않았다면 다음 오브젝트로 진행
            ++it;
        }
    }
}

void Layer::Release()
{
	for (auto& gameObject : m_GameObjects)
		SafeRelease(gameObject);

	m_GameObjects.clear();
}

void Layer::RemoveGameObject(GameObject* gameObject)
{
	if (nullptr == gameObject)
		return;

	gameObject->SetRemovalFlag(true);
}

std::list<GameObject*>& Layer::GetGameObjects()
{
    return m_GameObjects;
}
