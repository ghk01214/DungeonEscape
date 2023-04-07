#pragma once

#include "physx_utils.h"

class RaycastHit
{
public:
	RaycastHit();
public:
	physx::PxVec3 point;
	physx::PxVec3 normal;
	float distance{ -1.f };
	class Collider* collider = nullptr;
};
