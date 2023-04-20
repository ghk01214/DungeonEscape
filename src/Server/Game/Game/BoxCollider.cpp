#include "pch.h"
#include "BoxCollider.h"

using namespace physx;

PxGeometryHolder BoxCollider::CreateGeometry()
{
	return CreateBoxGeometry();
}

BoxCollider::BoxCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent)
	: Collider(ownerGameObject, ownerComponent, body), m_extent(extent)
{
}

BoxCollider::~BoxCollider()
{
}

Vec3 BoxCollider::GetExtent()
{
	return m_extent;
}

void BoxCollider::SetExtents(Vec3 extent)
{
	m_extent = extent;

	ResetShape();
}

physx::PxBoxGeometry BoxCollider::CreateBoxGeometry() const
{
	physx::PxBoxGeometry box;
	box.halfExtents = physx::PxVec3(m_extent.x, m_extent.y, m_extent.z) * 0.5;
	return box;
}
