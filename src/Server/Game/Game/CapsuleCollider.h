#pragma once

#include "physx_define.h"
#include "Collider.h"

class CapsuleCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	CapsuleCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent);
	~CapsuleCollider();

public:
	float GetRadius();
	float GetHalfHeight();
	void SetRadius(float value);
	void SetHalfHeight(float value);
	void SetExtent(Vec3 extent);

private:
	physx::PxCapsuleGeometry CreateCapsuleGeometry();

private:
	float m_radius = 0.5f;
	float m_halfHeight = 0.5f;
};

