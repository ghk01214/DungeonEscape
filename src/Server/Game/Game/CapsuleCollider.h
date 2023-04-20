#pragma once
#include "physx_utils.h"

class Collider;

class CapsuleCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	CapsuleCollider();
	~CapsuleCollider();

public:

	float GetRadius();
	float GetHalfHeight();
	void SetRadius(float value);
	void SetHalfHeight(float value);

private:
	physx::PxCapsuleGeometry CreateCapsuleGeometry(float radius, float halfHeight);

private:
	float m_radius = 0.5f;
	float m_halfHeight = 0.5f;
};

