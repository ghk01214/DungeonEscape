#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"

ObjectManager::ObjectManager()
{
}

void ObjectManager::Init()
{
    //Layer을 레벨 구분하여 관리하게 된다면 배열 할당에 사용 가능하기 때문에 빈 함수로 남깁니다.
}

void ObjectManager::Update(double timeDelta)
{
    for (auto& layer : m_Layers)
    {
        layer.second->Update(timeDelta);
    }
}

void ObjectManager::LateUpdate(double timeDelta)
{
    for (auto& layer : m_Layers)
    {
        layer.second->LateUpdate(timeDelta);
    }
}

void ObjectManager::Release()
{
    for (auto& layer : m_Layers)
    {
        SafeRelease(layer.second);
    }

    m_Layers.clear();
}

Layer* ObjectManager::GetLayer(const std::wstring& layerTag) const
{
    auto iter = m_Layers.find(layerTag);
    if (iter != m_Layers.end())
    {
        return iter->second;
    }
    return nullptr;
}

Layer* ObjectManager::AddLayer(const std::wstring& layerTag)
{
    Layer* newLayer = new Layer();
    m_Layers.emplace(std::make_pair(std::as_const(layerTag), newLayer));
    
    return newLayer;
}

bool ObjectManager::RemoveLayer(const std::wstring& layerTag)
{
    auto iter = m_Layers.find(layerTag);
    if (iter != m_Layers.end())
    {
        delete iter->second;
        m_Layers.erase(iter);
        return true;
    }
    return false;
}

template <typename T, typename... Args>
T* ObjectManager::AddGameObject(const std::wstring layerTag, Args&&... args)
{
    static_assert(std::is_base_of<GameObject, T>::value, "T must be derived from GameObject\n");
                                                        //부모클래스가 gameObject인지 재확인합니다.
    Layer* layer = GetLayer(layerTag);
    if (layer == nullptr)
    {
        return nullptr;
    }

    T* newGameObject = layer->AddGameObject<T>(std::forward<Args>(args)...);
    return newGameObject;
}
    

void ObjectManager::RemoveGameObject(const std::wstring layerTag, GameObject* gameObject)
{
    const auto& layer = GetLayer(layerTag);
    layer->RemoveGameObject(gameObject);
}

