#pragma once
#include "physx_utils.h"

class SphereCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	SphereCollider();
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



