#pragma once

#include "physx_define.h"
#include "Collider.h"

class Collider;

class SphereCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	SphereCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent);
	~SphereCollider();

public:

	float GetRadius() const;

	void SetRadius(float value);

private:

	physx::PxSphereGeometry CreateSphereGeometry() const;

	float GetBiggestLengthFromAbsVec3(float valueX, float valueY, float valueZ) const;

private:

	float m_radius = 0.5f;
};



