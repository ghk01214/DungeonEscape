#include "pch.h"
#include "BoxCollider.h"

using namespace physx;

PxGeometryHolder BoxCollider::CreateGeometry()
{
	return CreateBoxGeometry();
}

BoxCollider::BoxCollider()
{
}

BoxCollider::~BoxCollider()
{
}

float BoxCollider::GetExtentX()
{
	return m_extentX;
}

float BoxCollider::GetExtentY()
{
	return m_extentY;
}

float BoxCollider::GetExtentZ()
{
	return m_extentZ;
}

void BoxCollider::SetExtents(float extentX, float extentY, float extentZ)
{
	m_extentX = extentX;
	m_extentY = extentY;
	m_extentZ = extentZ;

	ResetShape();
}

physx::PxBoxGeometry BoxCollider::CreateBoxGeometry() const
{
	physx::PxBoxGeometry box;
	box.halfExtents = physx::PxVec3(m_extentX, m_extentY, m_extentZ) * 0.5;
	return box;
}
