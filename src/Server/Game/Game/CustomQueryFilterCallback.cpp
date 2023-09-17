#include "pch.h"
#include "CustomQueryFilterCallback.h"
#include "PhysDevice.h"
#include "Collider.h"
#include "RigidBody.h"

using namespace physx;

CustomQueryFilterCallback::CustomQueryFilterCallback(PhysicsQueryType queryType, bool queryOnce, RigidBody* ignoreBody)
{
    m_targets = 0xFFFFFFFF;
    m_queryType = queryType;
    m_hitType = queryOnce ? PxQueryHitType::eBLOCK : PxQueryHitType::eTOUCH;
    m_ignoreBody = ignoreBody;
}

CustomQueryFilterCallback::CustomQueryFilterCallback(PxU32 targetLayerBits, PhysicsQueryType queryType, bool queryOnce, RigidBody* ignoreBody)
{
    m_targets = targetLayerBits;
    m_queryType = queryType;
    m_hitType = queryOnce ? PxQueryHitType::eBLOCK : PxQueryHitType::eTOUCH;
    m_ignoreBody = ignoreBody;
}

PxQueryHitType::Enum CustomQueryFilterCallback::preFilter(
    const PxFilterData& filterData,
    const PxShape* shape,
    const PxRigidActor* actor,
    PxHitFlags& queryFlags)
{
    auto device = PhysDevice::GetInstance();

    Collider* collider = static_cast<Collider*>(shape->userData);
    RigidBody* rigidbody = static_cast<RigidBody*>(actor->userData);


    //m_ignoreBody가 nullptr일때 none반환
    if (m_ignoreBody && rigidbody == m_ignoreBody)
        return PxQueryHitType::eNONE;

    if ((1 << collider->GetLayerIndex()) & m_targets)
        return m_hitType;

    else
        return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum CustomQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
    return PxQueryHitType::eTOUCH;
}