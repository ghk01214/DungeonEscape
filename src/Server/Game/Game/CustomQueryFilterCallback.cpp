#include "pch.h"
#include "CustomQueryFilterCallback.h"

CustomQueryFilterCallback::CustomQueryFilterCallback(PhysicsQueryType queryType, bool queryOnce, RigidBody* ignoreBody)
{
    m_targets = 0xFFFFFFFF;
    m_queryType = queryType;
    m_hitType = queryOnce ? physx::PxQueryHitType::eBLOCK : physx::PxQueryHitType::eTOUCH;
    m_ignoreBody = ignoreBody;
}

CustomQueryFilterCallback::CustomQueryFilterCallback(physx::PxU32 targetLayerBits, PhysicsQueryType queryType, bool queryOnce, RigidBody* ignoreBody)
{
    m_targets = targetLayerBits;
    m_queryType = queryType;
    m_hitType = queryOnce ? physx::PxQueryHitType::eBLOCK : physx::PxQueryHitType::eTOUCH;
    m_ignoreBody = ignoreBody;
}

physx::PxQueryHitType::Enum CustomQueryFilterCallback::preFilter(
    const physx::PxFilterData& filterData,
    const physx::PxShape* shape,
    const physx::PxRigidActor* actor,
    physx::PxHitFlags& queryFlags)
{
    auto device = PhysDevice::GetInstance();

    Collider* collider = static_cast<Collider*>(shape->userData);
    RigidBody* rigidbody = static_cast<RigidBody*>(actor->userData);

    //m_ignoreBody가 nullptr일때 none반환
    if (m_ignoreBody && rigidbody == m_ignoreBody)
        return physx::PxQueryHitType::eNONE;

    if ((1 << collider->GetLayerIndex()) & m_targets)
        return m_hitType;
    else
        return physx::PxQueryHitType::eNONE;
}

physx::PxQueryHitType::Enum CustomQueryFilterCallback::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit)
{
    return physx::PxQueryHitType::eTOUCH;
}