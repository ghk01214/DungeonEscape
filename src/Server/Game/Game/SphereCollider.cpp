#include "pch.h"
#include "SphereCollider.h"

using namespace physx;

PxGeometryHolder SphereCollider::CreateGeometry()
{
    return CreateSphereGeometry();
}


SphereCollider::SphereCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent)
	: Collider(ownerGameObject, ownerComponent, body)
{
    int biggerRadius = (extent.x > extent.y) ? extent.x : extent.y;
    m_radius = (biggerRadius > extent.z) ? biggerRadius : extent.z;
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

PxSphereGeometry SphereCollider::CreateSphereGeometry() const
{
    PxSphereGeometry sphere;
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