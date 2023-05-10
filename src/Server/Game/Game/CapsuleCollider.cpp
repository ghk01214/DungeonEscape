#include "pch.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Transform.h"

using namespace physx;

PxGeometryHolder CapsuleCollider::CreateGeometry()
{
    return CreateCapsuleGeometry();
}

CapsuleCollider::CapsuleCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent)
    : Collider(ownerGameObject, ownerComponent, body), m_radius((extent.x > extent.y) ? extent.x : extent.y), m_halfHeight(extent.z)
{
}

CapsuleCollider::~CapsuleCollider()
{
}

float CapsuleCollider::GetRadius()
{
    return m_radius;
}

void CapsuleCollider::SetRadius(float value)
{
    m_radius = value;
    ResetShape();
}

float CapsuleCollider::GetHalfHeight()
{
    return m_halfHeight;
}

void CapsuleCollider::SetHalfHeight(float value)
{
    m_halfHeight = value;
    ResetShape();
}

void CapsuleCollider::SetExtent(Vec3 extent)
{
    m_radius = (extent.x > extent.y) ? extent.x : extent.y;
    m_halfHeight = extent.z;
}

PxCapsuleGeometry CapsuleCollider::CreateCapsuleGeometry()
{
    PxCapsuleGeometry capsule;
    capsule.radius = PxReal(m_radius);
    capsule.halfHeight = PxReal(m_halfHeight);
    return capsule;
}
