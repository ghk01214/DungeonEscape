#pragma once
#include "physx_utils.h"

class RigidBody;
enum class PhysicsQueryType;

class CustomQueryFilterCallback : public physx::PxQueryFilterCallback {
public:
	// Set all to target
	CustomQueryFilterCallback(PhysicsQueryType queryType, bool queryOnce, class RigidBody* ignoreBody = nullptr);

	// Set specific
	CustomQueryFilterCallback(physx::PxU32 targetLayerBits, PhysicsQueryType queryType, bool queryOnce, class RigidBody* ignoreBody = nullptr);

	virtual physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData,
		const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags) override;

	physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit) override;

private:
	physx::PxU32				m_targets{};
	RigidBody*					m_ignoreBody{};
	PhysicsQueryType			m_queryType{};
	physx::PxQueryHitType::Enum	m_hitType{};
};