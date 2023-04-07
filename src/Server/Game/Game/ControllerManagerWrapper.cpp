#include "pch.h"
#include "ControllerManagerWrapper.h"

void ControllerManagerWrapper::Init()
{
    auto phys = PhysDevice::GetInstance();
    m_scene = phys->GetScene();

    m_manager = PxCreateControllerManager(*m_scene);

    m_manager->setOverlapRecoveryModule(true); // Enable overlap recovery module
}

void ControllerManagerWrapper::CreateController()
{
    auto phys = PhysDevice::GetInstance();
    m_scene = phys->GetScene();

    // Create the capsule controller
    physx::PxCapsuleControllerDesc capsuleDesc;
    capsuleDesc.height = 4.5f;
    capsuleDesc.radius = 2.0f;
    capsuleDesc.material = phys->GetDefaultMaterial();
    capsuleDesc.position = physx::PxExtendedVec3(0.f, 35.f, 0.f);
    capsuleDesc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
    capsuleDesc.slopeLimit = cosf(physx::PxPi / 3.0f);        //60degress
    capsuleDesc.contactOffset = 0.001f;
    capsuleDesc.stepOffset = 0.1f;

    // CreateController + SetFlags
    physx::PxCapsuleController* controller = static_cast<physx::PxCapsuleController*>(m_manager->createController(capsuleDesc));
    physx::PxRigidDynamic* controllerActor = controller->getActor();
    controllerActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    //controllerActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
    controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING);

    // Lock the rotation of the actor along the axis perpendicular to the up direction
    //controllerActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);

    // WrappingClass
    ControllerWrapper* wrapper = new ControllerWrapper;
    wrapper->SetController(controller);
    controllerActor->userData = static_cast<void*>(wrapper);
    //controller의 멤버변수에 Rigidbody를 넣어서 보관하고, axis고정을 해서 move든 충돌이든 시도

    wrapper->SetDensity(1.0f);
    wrapper->SetRotationLockAxis(PhysicsAxis::All, true);

    // Place the controller to the vector
    static int serialNum = 0;
    wrapper->SetSerial(serialNum++);
    wrapper->Init();
    m_controllerWrappers.emplace_back(wrapper);
}

void ControllerManagerWrapper::UpdateControllers()
{
    for (auto& wrapper : m_controllerWrappers)
    {
        wrapper->Update();
    }
}

physx::PxControllerManager* ControllerManagerWrapper::GetControllerManager()
{
    return m_manager;
}
