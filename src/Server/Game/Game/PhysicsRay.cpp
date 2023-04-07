#include "pch.h"
#include "PhysicsRay.h"

PhysicsRay::PhysicsRay()
{
}

PhysicsRay::PhysicsRay(const physx::PxVec3& point, const physx::PxVec3& direction)
{
	this->point = point;
	this->direction = direction;
}

PhysicsRay::PhysicsRay(const physx::PxVec3& point, const physx::PxVec3& direction, float distance)
{
	this->point = point;
	this->direction = direction;
	this->distance = distance;
}