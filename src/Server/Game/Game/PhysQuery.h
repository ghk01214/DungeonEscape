﻿#pragma once
#include "physx_define.h"

class RigidBody;
class PhysicsRay;
class RaycastHit;

class PhysQuery
{
public:
    void Init();

public:
    bool Raycast(RaycastHit& hit, const PhysicsRay& ray, physx::PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody);
    bool RaycastOnce(RaycastHit& hit, const PhysicsRay& ray, physx::PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody);
    bool RaycastOld(const physx::PxVec3& origin, const physx::PxVec3& direction, physx::PxReal distance, physx::PxQueryFilterData filterData, physx::PxRaycastBuffer* hitBuffer);
    bool Overlap(const physx::PxGeometry& geometry, const physx::PxTransform& transform, physx::PxQueryFilterData filterData, physx::PxOverlapBuffer* hitBuffer);
    bool Sweep(const physx::PxGeometry& geometry, const physx::PxTransform& pose, const physx::PxVec3& unitDir, physx::PxReal distance, physx::PxQueryFilterData filterData, physx::PxSweepBuffer* hitBuffer);

private:
    const static physx::PxHitFlags bufferFlags;

    const static physx::PxQueryFilterData defaultFilterData;

    const static physx::PxQueryFilterData fastFilterData;
};

