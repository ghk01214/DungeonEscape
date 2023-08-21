#include "pch.h"
#include "PhysQuery.h"
#include "RaycastHit.h"
#include "PhysicsRay.h"
#include "CustomQueryFilterCallback.h"
#include "PhysDevice.h"

using namespace physx;

const PxHitFlags PhysQuery::bufferFlags =
PxHitFlag::ePOSITION |          // 충돌 위치
PxHitFlag::eNORMAL |            // 충돌 노멀
PxHitFlag::eFACE_INDEX |        // 충돌한 폴리곤의 인덱스
PxHitFlag::eMESH_BOTH_SIDES;    // 삼각형의 양쪽에서 적중할 수 있습니다.

const PxQueryFilterData PhysQuery::defaultFilterData
(
	PxQueryFlag::eDYNAMIC |         // dynamic 적중 설정
	PxQueryFlag::eSTATIC |          // static  적중 설정
	PxQueryFlag::ePREFILTER         // preFilter만 사용	
);

const PxQueryFilterData PhysQuery::fastFilterData
(
	PxQueryFlag::eDYNAMIC |         // dynamic 적중 설정
	PxQueryFlag::eSTATIC |          // static  적중 설정
	PxQueryFlag::ePREFILTER |       // preFilter만 사용	
	PxQueryFlag::eANY_HIT           // 첫 적중에 종료
	// 충돌 데이터가 없습니다.
);

void PhysQuery::Init()
{
}

bool PhysQuery::Raycast(RaycastHit& hit, const PhysicsRay& ray, PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();

	CustomQueryFilterCallback filterCallback(/*layerMask,*/ queryType, true, ignoreBody);
	PxRaycastBuffer hitBuffer;

	bool result = scene->raycast(ray.point, ray.direction, ray.distance,
		hitBuffer, PxHitFlag::eDEFAULT, defaultFilterData, &filterCallback);

	if (result)
	{
		const PxRaycastHit& pxHit = hitBuffer.getAnyHit(0);
		hit.point = pxHit.position;
		hit.normal = pxHit.normal;
		hit.distance = pxHit.distance;
		void* data = pxHit.shape->userData;
		hit.collider = static_cast<Collider*>(data);
	}

	return result;
}

bool PhysQuery::Raycast_MaskApplied(RaycastHit& hit, const PhysicsRay& ray, physx::PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();

	CustomQueryFilterCallback filterCallback(layerMask, queryType, true, ignoreBody);
	PxRaycastBuffer hitBuffer;

	bool result = scene->raycast(ray.point, ray.direction, ray.distance,
		hitBuffer, PxHitFlag::eDEFAULT, defaultFilterData, &filterCallback);

	if (result)
	{
		const PxRaycastHit& pxHit = hitBuffer.getAnyHit(0);
		hit.point = pxHit.position;
		hit.normal = pxHit.normal;
		hit.distance = pxHit.distance;
		void* data = pxHit.shape->userData;
		hit.collider = static_cast<Collider*>(data);
	}

	return result;
}

bool PhysQuery::RaycastOnce(RaycastHit& hit, const PhysicsRay& ray, physx::PxU32 layerMask, PhysicsQueryType queryType, RigidBody* ignoreBody)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();

	CustomQueryFilterCallback filterCallback(/*layerMask,*/ queryType, true, ignoreBody);
	PxRaycastBuffer hitBuffer;

	bool result = scene->raycast(ray.point, ray.direction, ray.distance,
		hitBuffer, PxHitFlag::eMESH_ANY, defaultFilterData, &filterCallback);

	if (result)
	{
		const PxRaycastHit& pxHit = hitBuffer.getAnyHit(0);
		hit.point = pxHit.position;
		hit.normal = pxHit.normal;
		hit.distance = pxHit.distance;
		void* data = pxHit.shape->userData;
		hit.collider = static_cast<Collider*>(data);
	}

	return result;
}

bool PhysQuery::RaycastOld(const PxVec3& origin, const PxVec3& direction, PxReal distance, PxQueryFilterData filterData, PxRaycastBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->raycast(origin, direction, distance, *hitBuffer, PxHitFlag::eDEFAULT, filterData);
}

bool PhysQuery::Overlap(const PxGeometry& geometry, const PxTransform& transform, PxQueryFilterData filterData, PxOverlapBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->overlap(geometry, transform, *hitBuffer, filterData);
}

bool PhysQuery::Sweep(const PxGeometry& geometry, const PxTransform& pose, const PxVec3& unitDir, PxReal distance, PxQueryFilterData filterData, PxSweepBuffer* hitBuffer)
{
	auto device = PhysDevice::GetInstance();
	auto scene = device->GetScene();
	return scene->sweep(geometry, pose, unitDir, distance, *hitBuffer, PxHitFlag::eDEFAULT, filterData);
}
