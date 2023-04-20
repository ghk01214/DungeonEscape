#pragma once

#include "physx_define.h"
#include "Collider.h"

class BoxCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	BoxCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent);
	~BoxCollider();

public:
	Vec3 GetExtent();
	void SetExtents(Vec3 extent);

private:
	physx::PxBoxGeometry CreateBoxGeometry() const;

private:
	Vec3 m_extent;
};

