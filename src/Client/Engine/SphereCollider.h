#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

	void SetRadius(float radius) { m_radius = radius; }
	void SetCenter(Vec3 center) { m_center = center; }

private:
	// Local 기준
	float		m_radius = 1.f;
	Vec3		m_center = Vec3(0, 0, 0);

	BoundingSphere m_boundingSphere;
};

