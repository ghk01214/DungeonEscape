#include "pch.h"
#include "Transform.h"

void Transform::Init()
{
}

void Transform::Release()
{
}

Transform::Transform(GameObject* ownerGameObject, Component* ownerComponent, const Vec3& position, const Quat& rotation, const Vec3& scale)
    : Component(ownerGameObject, ownerComponent), m_Pos(position), m_Rot(rotation), m_Scale(scale)
{
}

void Transform::SetPosition(const Vec3& v)
{
    m_Pos = v;
}

void Transform::SetPosition(float x, float y, float z)
{
    m_Pos.x = x;
    m_Pos.y = y;
    m_Pos.z = z;
}

void Transform::SetPosition(const DirectX::XMFLOAT3& position)
{
    m_Pos = position;
}

Vec3 Transform::GetPosition() const
{
    return m_Pos;
}

void Transform::SetRotation(const Quat& q)
{
    m_Rot = q;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    m_Rot = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
   
}

void Transform::SetRotation(const DirectX::XMFLOAT3& rotation)
{
    m_Rot = DirectX::XMQuaternionRotationRollPitchYaw(rotation.y, rotation.x, rotation.z);
}

void Transform::SetRotation(const DirectX::XMFLOAT4& rotation)
{
    m_Rot = Quat(rotation);
}

Quat Transform::GetRotation() const
{
    return m_Rot;
}

void Transform::SetScale(const Vec3& v)
{
    m_Scale = v;
}

void Transform::SetScale(float x, float y, float z)
{
    m_Scale.x = x;
    m_Scale.y = y;
    m_Scale.z = z;
}

void Transform::SetScale(const DirectX::XMFLOAT3& scale)
{
    m_Scale = scale;
}

void Transform::SetScale(float scale)
{
    m_Scale = Vec3(scale, scale, scale);
}

Vec3 Transform::GetScale() const
{
    return m_Scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() const
{
    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_Rot)) *
        DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z));
    return worldMatrix;
}
