#pragma once

#include "physx_define.h"

class PhysicsRay
{
public:
	PhysicsRay();
	PhysicsRay(const physx::PxVec3& point, const physx::PxVec3& direction);
	PhysicsRay(const physx::PxVec3& point, const physx::PxVec3& direction, float distance);

public:
	physx::PxVec3 point;
	physx::PxVec3 direction;
	float distance = FLT_MAX;
};

