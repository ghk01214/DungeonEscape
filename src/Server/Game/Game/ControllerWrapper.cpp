#include "pch.h"
#include "ControllerWrapper.h"
#include "PhysDevice.h"
#include "PhysQuery.h"

using namespace physx;

#pragma region CustomController가 고장나지 않으면 사용하지 않음.

//ControllerWrapper::ControllerWrapper()
//{
//}
//
//ControllerWrapper::~ControllerWrapper()
//{
//}
//
//void ControllerWrapper::Init()
//{
//    m_body = m_controller->getActor();
//    //PxShape* shape = nullptr;
//    //m_body->getShapes(&shape, 1);
//
//    //PxGeometryHolder holder = shape->getGeometry();
//    //if (PxGeometryType::eCAPSULE == holder.getType())                                      /////////////////////////여기부터.
//    //{
//    //    PxCapsuleGeometry& capsule = holder.capsule();
//    //    m_halfHeight = capsule.halfHeight;
//    //    m_radius = capsule.radius;
//    //    float controllerRaidus = m_controller->getRadius();
//    //    float controllerHeight = m_controller->getHeight();
//    //    cout << " hello" << endl;
//    //}
//    SetKinematic(true);
//}
//
//void ControllerWrapper::MoveSample()
//{
//    float timeStep = 1.0f / PX_SIM_FRAMECNT;
//
//    PxRaycastBuffer buffer;
//    bool onGround = UpdateOnGround(&buffer);
//
//    //jump calculation
//    PxVec3 jumpMovements(0.f, 0.f, 0.f);
//    float jumpSpeed = 15.0f;  // adjust this to control the height of the jump
//    static float jumpTime = 0.f;
//    static bool jump = false;
//
//    //if (InputDevice::GetInstance()->GetKey(Key::Space) && onGround)
//    //{
//    //    m_controller->move(PxVec3(0.f, 0.1, 0.f), 0.001f, timeStep, PxControllerFilters(), nullptr);
//    //    onGround = false;               // to not fall into if(onGround) immediately
//    //    jump = true;
//    //    jumpTime = 0.f;
//    //}
//    if (jump)
//    {
//        jumpTime += timeStep;
//        float jumpDisplacement = jumpSpeed * jumpTime - 0.5f * 9.8f * jumpTime * jumpTime;
//        if (onGround)
//        {
//            jump = false;
//            jumpTime = 0.f;
//        }
//        else
//        {
//            jumpMovements.y = jumpDisplacement;
//        }
//    }
//
//
//    PxVec3 inputMovements(0.f, 0.f, 0.f);
//    float inputValue = 1.f;
//
//    //if (InputDevice::GetInstance()->GetKey(Key::A))
//    //{
//    //    inputMovements.x -= inputValue;
//    //}
//    //if (InputDevice::GetInstance()->GetKey(Key::D))
//    //{
//    //    inputMovements.x += inputValue;
//    //}
//    //if (InputDevice::GetInstance()->GetKey(Key::S))
//    //{
//    //    inputMovements.z -= inputValue;
//    //}
//    //if (InputDevice::GetInstance()->GetKey(Key::W))
//    //{
//    //    inputMovements.z += inputValue;
//    //}
//
//
//    //gravity calculation
//    static float midAirDuration = 0.f;
//    if (!onGround && !jump)
//        midAirDuration += timeStep;
//    else if (onGround)
//        midAirDuration = 0.f;
//
//    PxVec3 gravity(0.f, 0.f, 0.f);
//    if (!jump)
//    {
//        // only works when falling without jumping.
//        float maxFallingSpeed = 9.8f;
//        float timeToMaxSpeed = 2.0f;                                                    // time request for full fall speed
//        float t = min(1.0f, midAirDuration / timeToMaxSpeed);                           // normalize midair time by [0, 1]
//        float fallingSpeed = maxFallingSpeed * (1.0f - exp(-t * t * 4.0f));             // modify 4 for slower/qucker falling speed
//
//        gravity.y = -fallingSpeed;
//        //cout << fallingSpeed << endl;
//    }
//
//
//    //combine movements
//    PxVec3 displacement = (inputMovements + gravity + jumpMovements) * timeStep;
//
//    PxControllerCollisionFlags collisionFlags = m_controller->move(displacement, 0.001f, timeStep, PxControllerFilters(), nullptr);
//}
//
//void ControllerWrapper::MoveByKinematic()
//{
//    //PxVec3 inputMovements(0.f, 0.f, 0.f);
//    //float inputValue = 1.f;
//
//    //PxVec3 gravity(0.f, -9.81f * 0.5f, 0.f);
//
//    //float timeStep = 1.0f / PX_SIM_FRAMECNT;
//
//    //if (InputDevice::GetInstance()->GetKey(Key::A))
//    //{
//    //    inputMovements.x -= inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::D))
//    //{
//    //    inputMovements.x += inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::S))
//    //{
//    //    inputMovements.z -= inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::W))
//    //{
//    //    inputMovements.z += inputValue;
//    //}
//
//    ////combine movements
//    //PxVec3 displacement = (inputMovements + gravity) * timeStep;
//
//    //cout << "MoveByKinematic : before moving" << endl;
//    //cout << m_controller->getFootPosition().x << "," << m_controller->getFootPosition().y << "," << m_controller->getFootPosition().z << endl;
//
//    //PxControllerCollisionFlags collisionFlags = m_controller->move(displacement, 0.001f, timeStep, PxControllerFilters(), nullptr);
//
//    //cout << "MoveByKinematic : after moving" << endl;
//    //cout << m_controller->getFootPosition().x << "," << m_controller->getFootPosition().y << "," << m_controller->getFootPosition().z << endl;
//}
//
//void ControllerWrapper::MoveByNonKinematic()
//{
//}
//
//void ControllerWrapper::SlopeMovements()
//{
//    PxVec3 inputMovements(0.f, 0.f, 0.f);
//    float inputValue = 1.f;
//    float timeStep = 1.f / PX_SIM_FRAMECNT;
//
//    //if (InputDevice::GetInstance()->GetKey(Key::A))
//    //{
//    //    inputMovements.x -= inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::D))
//    //{
//    //    inputMovements.x += inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::S))
//    //{
//    //    inputMovements.z -= inputValue;
//    //}
//    //else if (InputDevice::GetInstance()->GetKey(Key::W))
//    //{
//    //    inputMovements.z += inputValue;
//    //}
//
//    // Check if the controller is on the ground
//    PxControllerState state;
//    m_controller->getState(state);
//    bool isOnGround = (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) ||
//        (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_SIDES);
//
//#pragma region raycastDown
//    PxRaycastBuffer hitBuffer;
//    PxQueryFilterData filterData;
//    PxExtendedVec3 currentFootPos = m_controller->getFootPosition();
//    //default contact offset of rigidBody
//    PxVec3 origin = PxVec3(currentFootPos.x, currentFootPos.y + 0.2f, currentFootPos.z);
//    PxVec3 direction(0.0f, -1.0f, 0.0f);
//    PxReal distance = 100.0f;
//    auto query = PhysDevice::GetInstance()->GetQuery();
//    bool hitGround = query->RaycastOld(origin, direction, distance, filterData, &hitBuffer);
//
//    // If the raycast hit the ground, set the foot position
//    if (hitGround)
//    {
//        PxReal hitDistance = hitBuffer.block.distance;
//        std::cout << "raycast : " << hitDistance << "\t flag = " << isOnGround << std::endl;
//    }
//#pragma endregion
//
//    // calculate gravity magnitude based on elapsed time
//    static float gravityMagnitude = 0.f;
//    if (isOnGround)
//    {
//        gravityMagnitude = 0.f;
//    }
//    else
//    {
//        gravityMagnitude += timeStep * 9.81f;
//        gravityMagnitude = min(gravityMagnitude, 9.81f);
//    }
//
//    PxVec3 gravity(0.f, -gravityMagnitude, 0.f);
//
//    // movements + grav
//    PxVec3 displacement = (inputMovements + gravity) * timeStep;
//
//    PxControllerCollisionFlags collisionFlags = m_controller->move(displacement, 0.001f, timeStep, PxControllerFilters(), nullptr);
//}
//
//void ControllerWrapper::Jump()
//{
//    //if (InputDevice::GetInstance()->GetKey(Key::Space))
//    //{
//    //    if (UpdateOnGround())
//    //    {
//    //        float timeStep = 1.0f / PX_SIM_FRAMECNT;
//    //        float jumpStrength = 3.f;
//
//    //        PxVec3 displacement = PxVec3(0.f, 1.f, 0.f) * timeStep;
//
//    //        PxControllerCollisionFlags collisionFlags = m_controller->move(displacement, 0.001f, timeStep, PxControllerFilters(), nullptr);
//
//    //        SetKinematic(false);
//    //        m_body->addForce(PxVec3(0, jumpStrength, 0), PxForceMode::eIMPULSE);
//    //    }
//    //}
//}
//
//bool ControllerWrapper::UpdateOnGround(PxRaycastBuffer* buffer)
//{
//    //추후 경사면 추가 실험
//    // Check if the controller is on the ground
//    PxControllerState state;
//    m_controller->getState(state);
//    bool isOnGround = (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) ||
//        (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_SIDES);
//
//#pragma region raycastDown
//    PxQueryFilterData filterData;
//
//    PxExtendedVec3 currentFootPos = m_controller->getFootPosition();
//
//    // edit y value accordingly. slightly bigger than the capsuleDesc.contactOffset
//    PxReal contactOffset = 0.01f;
//
//    PxVec3 origin = PxVec3(currentFootPos.x, currentFootPos.y + contactOffset, currentFootPos.z);
//    PxVec3 direction(0.0f, -1.0f, 0.0f);
//    PxReal distance = 100.0f;
//    auto query = PhysDevice::GetInstance()->GetQuery();
//    bool hitGround = query->RaycastOld(origin, direction, distance, filterData, buffer);
//
//    // If the raycast hit the ground, set the foot position
//    if (hitGround)
//    {
//        PxReal hitDistance = buffer->block.distance;
//        //cout << hitDistance << endl;
//        if (isOnGround || hitDistance < 0.01f)
//        {
//            return true;
//        }
//    }
//
//    return false;
//#pragma endregion
//}
//
//void ControllerWrapper::UpdateKinematic()
//{
//    //bool onGround = UpdateOnGround();
//    //bool isKinematic = IsKinematic();
//
//
//    //if (onGround && !isKinematic)       // on the surface + is not kinematic
//    //    SetKinematic(true);
//    //else if(!onGround && isKinematic)   // on mid air + is kinematic
//    //    SetKinematic(false);
//}
//
//void ControllerWrapper::Update()
//{
//    MoveSample();
//
//    //UpdateKinematic();
//
//}
//
//PxCapsuleController* ControllerWrapper::GetController()
//{
//    return m_controller;
//}
//
//void ControllerWrapper::SetController(PxCapsuleController* controller)
//{
//    m_controller = controller;
//}
//
//int ControllerWrapper::GetSerial()
//{
//    return m_serial;
//}
//
//void ControllerWrapper::SetDensity(float density)
//{
//    m_density = density;
//    if (m_controller)
//    {
//        auto actor = m_controller->getActor();
//        if (actor)
//        {
//            actor->setMass(density > 0.0f ? 1.0f / density : PX_MAX_F32);
//            actor->setMassSpaceInertiaTensor(PxVec3(1.0f, 1.0f, 1.0f) * actor->getMass());
//        }
//    }
//}
//
//float ControllerWrapper::GetDensity()
//{
//    return m_density;
//}
//
//void ControllerWrapper::SetRotationLockAxis(PhysicsAxis axes, bool value)
//{
//    PxU32 flag = (PxU32)axes << 3;
//
//    PxRigidDynamic* body = m_controller->getActor();
//
//    body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum(flag), value);
//}
//
//bool ControllerWrapper::IsKinematic() const
//{
//    PxRigidDynamic* body = m_controller->getActor();
//    return body->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
//}
//
//void ControllerWrapper::SetKinematic(bool value)
//{
//    m_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
//    if (!value)
//        m_body->wakeUp();
//}
//
//void ControllerWrapper::SetSerial(int serial)
//{
//    m_serial = serial;
//}

#pragma endregion 