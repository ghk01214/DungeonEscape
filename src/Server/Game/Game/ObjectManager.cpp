#include "pch.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"

ImplementSingletone(ObjectManager);

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
    m_Layers.emplace(std::make_pair(layerTag, newLayer));
    
    return newLayer;
}

const std::unordered_map<std::wstring, Layer*>& ObjectManager::GetLayers() const
{
    return m_Layers;
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

void ObjectManager::TimeUpdate_PlayerTrigger(double timeDelta)
{
    auto layer = ObjectManager::GetInstance()->GetLayer(L"Layer_Player");
    if (!layer)
        return;

    auto list = layer->GetGameObjects();

    if (list.size() < 1)
        return;

    /*for (auto& ptr : list)
    {
        auto player = dynamic_cast<Player*>(ptr);
        if (player)
            continue;

        player->TimeUpdate_Trigger(timeDelta);
    }*/
}

void ObjectManager::RemoveGameObjectFromLayer(const std::wstring& layerTag, GameObject* gameObject)
{
    const auto& layer = GetLayer(layerTag);
    layer->RemoveGameObject(gameObject);
}

