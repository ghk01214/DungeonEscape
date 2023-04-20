#include "pch.h"
#include "ControllerManagerWrapper.h"
#include "PhysDevice.h"
#include "ControllerWrapper.h"

using namespace physx;

#pragma region CustomController가 고장나지 않으면 사용하지 않음.

//void ControllerManagerWrapper::Init()
//{
//    auto phys = PhysDevice::GetInstance();
//    m_scene = phys->GetScene();
//
//    m_manager = PxCreateControllerManager(*m_scene);
//
//    m_manager->setOverlapRecoveryModule(true); // Enable overlap recovery module
//}
//
//void ControllerManagerWrapper::CreateController()
//{
//    auto phys = PhysDevice::GetInstance();
//    m_scene = phys->GetScene();
//
//    // Create the capsule controller
//    PxCapsuleControllerDesc capsuleDesc;
//    capsuleDesc.height = 4.5f;
//    capsuleDesc.radius = 2.0f;
//    capsuleDesc.material = phys->GetDefaultMaterial();
//    capsuleDesc.position = PxExtendedVec3(0.f, 35.f, 0.f);
//    capsuleDesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
//    capsuleDesc.slopeLimit = cosf(PxPi / 3.0f);        //60degress
//    capsuleDesc.contactOffset = 0.001f;
//    capsuleDesc.stepOffset = 0.1f;
//
//    // CreateController + SetFlags
//   PxCapsuleController* controller = static_cast<PxCapsuleController*>(m_manager->createController(capsuleDesc));
//   PxRigidDynamic* controllerActor = controller->getActor();
//    controllerActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
//    //controllerActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
//    controller->setNonWalkableMode(PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING);
//
//    // Lock the rotation of the actor along the axis perpendicular to the up direction
//    //controllerActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
//
//    // WrappingClass
//    ControllerWrapper* wrapper = new ControllerWrapper;
//    wrapper->SetController(controller);
//    controllerActor->userData = static_cast<void*>(wrapper);
//    //controller의 멤버변수에 Rigidbody를 넣어서 보관하고, axis고정을 해서 move든 충돌이든 시도
//
//    wrapper->SetDensity(1.0f);
//    wrapper->SetRotationLockAxis(PhysicsAxis::All, true);
//
//    // Place the controller to the vector
//    static int serialNum = 0;
//    wrapper->SetSerial(serialNum++);
//    wrapper->Init();
//    m_controllerWrappers.emplace_back(wrapper);
//}
//
//void ControllerManagerWrapper::UpdateControllers()
//{
//    for (auto& wrapper : m_controllerWrappers)
//    {
//        wrapper->Update();
//    }
//}
//
//PxControllerManager* ControllerManagerWrapper::GetControllerManager()
//{
//    return m_manager;
//}

#pragma endregion