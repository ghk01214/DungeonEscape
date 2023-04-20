#include "pch.h"
#include "SphereCollider.h"

physx::PxGeometryHolder SphereCollider::CreateGeometry()
{
    return CreateSphereGeometry();
}

SphereCollider::SphereCollider()
{
}

SphereCollider::~SphereCollider()
{
}

float SphereCollider::GetRadius() const
{
    return m_radius;
}

void SphereCollider::SetRadius(float value)
{
    m_radius = value;
    ResetShape();
}

physx::PxSphereGeometry SphereCollider::CreateSphereGeometry() const
{
    physx::PxSphereGeometry sphere;
    //transform scale 생긴이후
    sphere.radius = m_radius;                           //* GetBiggestLengthFromAbsVec3(transform->scale);
    return sphere;
}

float SphereCollider::GetBiggestLengthFromAbsVec3(float valueX, float valueY, float valueZ) const
{
    float maxNum = abs(valueX);
    maxNum = max(maxNum, abs(valueY));
    maxNum = max(maxNum, abs(valueZ));
    return maxNum;
}