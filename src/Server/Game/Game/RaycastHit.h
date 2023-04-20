#pragma once

#include "physx_define.h"

class Collider;

class RaycastHit
{
public:
	RaycastHit();
public:
	physx::PxVec3 point;
	physx::PxVec3 normal;
	float distance{ -1.f };
	Collider* collider = nullptr;
};
