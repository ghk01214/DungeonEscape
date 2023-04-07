#include "pch.h"
#include "PhysQuery.h"
#include "RaycastHit.h"
#include "PhysicsRay.h"
#include "CustomQueryFilterCallback.h"

const physx::PxHitFlags PhysQuery::bufferFlags =
physx::PxHitFlag::ePOSITION |          // 충돌 위치
physx::PxHitFlag::eNORMAL |            // 충돌 노멀
physx::PxHitFlag::eFACE_INDEX |        // 충돌한 폴리곤의 인덱스
physx::PxHitFlag::eMESH_BOTH_SIDES;    // 삼각형의 양쪽에서 적중할 수 있습니다.

const physx::PxQueryFilterData PhysQuery::defaultFilterData
(
	physx::PxQueryFlag::eDYNAMIC |         // dynamic 적중 설정
	physx::PxQueryFlag::eSTATIC |          // static  적중 설정
	physx::PxQueryFlag::ePREFILTER         // preFilter만 사용	
);

const physx::PxQueryFilterData PhysQuery::fastFilterData
(
	physx::PxQueryFlag::eDYNAMIC |         // dynamic 적중 설정
	physx::PxQueryFlag::eSTATIC |          // static  적중 설정
	physx::PxQueryFlag::ePREFILTER |       // preFilter만 사용	
	physx::PxQueryFlag::eANY_HIT           // 첫 적중에 종료
	// 충돌 데이터가 없습니다.
);

void PhysQuery::Init()
{
}

bool PhysQuery::Raycast(RaycastHit& hit, const PhysicsRay& ray, physx::PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();

	CustomQueryFilterCallback filterCallback(/*layerMask,*/ queryType, true, ignoreBody);
	physx::PxRaycastBuffer hitBuffer;

	bool result = scene->raycast(ray.point, ray.direction, ray.distance,
		hitBuffer, physx::PxHitFlag::eDEFAULT, defaultFilterData, &filterCallback);

	if (result)
	{
		const physx::PxRaycastHit& pxHit = hitBuffer.getAnyHit(0);
		hit.point = pxHit.position;
		hit.normal = pxHit.normal;
		hit.distance = pxHit.distance;
		void* data = pxHit.shape->userData;
		hit.collider = static_cast<Collider*>(data);
	}

	return result;
}

bool PhysQuery::RaycastOld(const physx::PxVec3& origin, const physx::PxVec3& direction, physx::PxReal distance, physx::PxQueryFilterData filterData, physx::PxRaycastBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->raycast(origin, direction, distance, *hitBuffer, physx::PxHitFlag::eDEFAULT, filterData);
}

bool PhysQuery::Overlap(const physx::PxGeometry& geometry, const physx::PxTransform& transform, physx::PxQueryFilterData filterData, physx::PxOverlapBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->overlap(geometry, transform, *hitBuffer, filterData);
}

bool PhysQuery::Sweep(const physx::PxGeometry& geometry, const physx::PxTransform& pose, const physx::PxVec3& unitDir, physx::PxReal distance, physx::PxQueryFilterData filterData, physx::PxSweepBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->sweep(geometry, pose, unitDir, distance, *hitBuffer, physx::PxHitFlag::eDEFAULT, filterData);
}
