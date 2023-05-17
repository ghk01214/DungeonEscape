#pragma once
#include "physx_define.h"

class CollisionPairInfo;
class Collider;

class CustomSimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
    // Override the other callback methods with empty implementations
    virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) { }
    virtual void onWake(physx::PxActor**, physx::PxU32) { }
    virtual void onSleep(physx::PxActor**, physx::PxU32) { }

    virtual void onTrigger(physx::PxTriggerPair*, physx::PxU32) {}

    // scene->simulate 이후 후처리
    virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) { }

    // 충돌시 반환
    virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

public:
    void Notify();
    void ClearVector();

public:
    std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionEnter;
    std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionStay;
    std::vector<std::shared_ptr<CollisionPairInfo>> m_CollisionExit;
};

