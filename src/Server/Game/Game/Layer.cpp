#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

void Layer::Init()
{
	//objectList를 레벨 구분하여 관리하게 된다면 배열 할당에 사용 가능하기 때문에 빈 함수로 남깁니다.
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

//template <typename T, typename... Args>
//T* Layer::AddGameObject(Args&&... args)
//{
//	static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject\n");
//													//부모클래스가 gameObject인지 재확인합니다.
//	T* newGameObject = new T();
//	newGameObject->Init(std::forward<Args>(args)...);
//	m_GameObjects.push_back(newGameObject);
//
//	return newGameObject;
//}
