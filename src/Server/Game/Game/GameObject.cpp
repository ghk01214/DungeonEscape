#include "pch.h"
#include "GameObject.h"
#include "Component.h"

GameObject::~GameObject()
{
    Release();
}

void GameObject::Release()
{
    for (auto& kv : m_Components)
    {
        SafeRelease(kv.second);
    }
    m_Components.clear();
}

bool GameObject::GetRemovalFlag()
{
    return m_RemovalFlag;
}

void GameObject::SetRemovalFlag(bool value)
{
    m_RemovalFlag = value;
}

template<typename T>
T* GameObject::GetComponent(const std::wstring componentName)
{
    auto it = m_Components.find(componentName);
    if (it == m_Components.end())
    {
        return nullptr;
    }
    return static_cast<T*>(it->second);
}

template<typename T>
T* GameObject::AddComponent(const std::wstring componentName)
{
    if (GetComponent<T>(componentName) != nullptr)
    {
        throw std::runtime_error("Component already exists\n");
    }

    T* component = new T(this);
    m_Components[componentName] = component;
    component->Init();
    return component;
}

template<typename T>
void GameObject::RemoveComponent(const std::wstring componentName)
{
    auto it = m_Components.find(componentName);
    if (it != m_Components.end())
    {
        SafeRelease(it->second);
        m_Components.erase(it);
    }
}
