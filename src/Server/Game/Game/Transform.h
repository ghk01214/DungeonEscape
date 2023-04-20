#pragma once

#include <DirectXMath.h>
#include "Component.h"

class GameObject;

class Transform : public Component
{
public:
    void Init() override;
    void Release() override;
public:
    Transform(GameObject* ownerGameObject, Component* ownerComponent, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));

    // Position
    void SetPosition(const Vec3& v);
    void SetPosition(float x, float y, float z);
    void SetPosition(const DirectX::XMFLOAT3& position);
    Vec3 GetPosition() const;

    // Rotation (in radians)
    void SetRotation(const Quat& q);
    void SetRotation(float pitch, float yaw, float roll);
    void SetRotation(const DirectX::XMFLOAT3& rotation);
    void SetRotation(const DirectX::XMFLOAT4& rotation);
    Quat GetRotation() const;

    // Scale
    void SetScale(const Vec3& v);
    void SetScale(float x, float y, float z);
    void SetScale(const DirectX::XMFLOAT3& scale);
    void SetScale(float scale);
    Vec3 GetScale() const;

    // Get world matrix
    DirectX::XMFLOAT4X4 GetWorldMatrix() const;

private:
    Vec3 m_Pos{};
    Quat m_Rot{};   //default 0,0,0,1
    Vec3 m_Scale{1.f, 1.f, 1.f};

};
