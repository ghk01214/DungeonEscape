#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "RigidBody.h"

GameObject::GameObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
{
    m_transform = AddComponent<Transform>(L"Transform" , position, rotation, scale);
}

GameObject::~GameObject()
{
    //Release();
}

void GameObject::Init()
{
}

void GameObject::Update(double timeDelta)
{
    if (m_removeReserved < 0)
        return;

    if (m_removeReserved == RESERVED)
    {
        auto body = GetComponent<RigidBody>(L"RigidBody");
        if (body == nullptr)
            return;
        body->ExcludeFromSimulation(true);
    }
    
    m_removeReserved -= 1;

    if (m_removeReserved == 0)
        SetRemovalFlag(true);
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

bool GameObject::AccessAuthorized()
{
    if (m_removeReserved < 0)
        return true;
    else
        false;
}

bool GameObject::GetRemovalFlag()
{
    return m_removalFlag;
}

void GameObject::SetRemovalFlag(bool value)
{
    m_removalFlag = value;
}

void GameObject::SetRemoveReserved()
{
    m_removeReserved = RESERVED;
}

Transform* GameObject::GetTransform()
{
    return m_transform;
}

void GameObject::SetID(int32_t id)
{
    m_id = id;
}

void GameObject::SetName(const std::wstring& name)
{
    m_name = name;
}

void GameObject::SetObjectType(server::OBJECT_TYPE objType)
{
    m_objType = objType;
}

void GameObject::SetFBXType(server::FBX_TYPE fbxType)
{
    m_fbxType = fbxType;
}

void GameObject::StartSendTransform()
{
    m_startSendTransform = true;
}
