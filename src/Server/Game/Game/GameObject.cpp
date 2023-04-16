#include "pch.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"


GameObject::GameObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
    : m_transform(new Transform(this, nullptr, position, rotation, scale))
{
}

GameObject::~GameObject()
{
    Release();
    SafeRelease(m_transform);
}

void GameObject::Release()
{
    for (auto& kv : m_components)
    {
        SafeRelease(kv.second);
    }
    m_components.clear();
}

Transform* GameObject::getTransform()
{
    return m_transform;
}

bool GameObject::GetRemovalFlag()
{
    return m_removalFlag;
}

void GameObject::SetRemovalFlag(bool value)
{
    m_removalFlag = value;
}

template<typename T>
T* GameObject::GetComponent(const std::wstring componentName)
{
    auto it = m_components.find(componentName);
    if (it == m_components.end())
    {
        return nullptr;
    }
    return static_cast<T*>(it->second);
}

template<typename T, typename... Args>
T* GameObject::AddComponent(const std::wstring componentName, Args&&... args)
{
    if (GetComponent<T>(componentName) != nullptr)
    {
        throw std::runtime_error("Component already exists\n");
    }

    T* component = new T(this, nullptr, std::forward<Args>(args)...);
    m_components[componentName] = component;
    component->Init();
    return component;
}

void GameObject::RemoveComponent(const std::wstring componentName)
{
    auto it = m_components.find(componentName);
    if (it != m_components.end())
    {
        SafeRelease(it->second);
        m_components.erase(it);
    }
}
