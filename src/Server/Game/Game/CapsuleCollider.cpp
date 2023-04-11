#include "pch.h"
#include "CapsuleCollider.h"

using namespace physx;

PxGeometryHolder CapsuleCollider::CreateGeometry()
{
    return CreateCapsuleGeometry(m_radius, m_halfHeight);
}

CapsuleCollider::CapsuleCollider()
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

PxCapsuleGeometry CapsuleCollider::CreateCapsuleGeometry(float radius, float halfHeight)
{
    PxCapsuleGeometry capsule;
    capsule.radius = m_radius;
    capsule.halfHeight = m_halfHeight;
    return capsule;
}
