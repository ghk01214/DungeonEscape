#include "pch.h"
#include "GameObject.h"
#include "Transform.h"


GameObject::GameObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
{
    m_transform = AddComponent<Transform>(L"Transform" , position, rotation, scale);
}

GameObject::~GameObject()
{
    Release();
}

void GameObject::Init()
{
}

void GameObject::Release()
{
    for (auto& kv : m_components)
    {
        SafeRelease(kv.second);
    }
    m_components.clear();
    m_transform = nullptr;
}

bool GameObject::GetRemovalFlag()
{
    return m_removalFlag;
}

void GameObject::SetRemovalFlag(bool value)
{
    m_removalFlag = value;
}

Transform* GameObject::GetTransform()
{
    return m_transform;
}
