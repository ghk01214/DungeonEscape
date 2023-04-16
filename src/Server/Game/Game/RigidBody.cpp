#include "pch.h"
#include "RigidBody.h"
#include "PhysDevice.h"
#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshCollider.h"


using namespace physx;

RigidBody::RigidBody(GameObject* ownerGameObject, Component* ownerComponent)
	: Component(ownerGameObject, ownerComponent)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::Init()
{
	auto device = PhysDevice::GetInstance();

	PxTransform pose(PxIdentity);

	m_body = device->GetPhysics()->createRigidDynamic(pose);
	m_body->setMass(10);
	m_body->userData = this;

	ApplyFlags();

	device->GetScene()->addActor(*m_body);

	SetTransfrom(m_ownerGameObject->getTransform());
}

void RigidBody::Release()
{
	auto it = m_colliders.begin();
	while (it != m_colliders.end())
	{
		//Mesh는 pxConvexMesh 메모리 해제를 위해 MeshCollider::Release()를 호출해야함
		auto meshptr = dynamic_cast<MeshCollider*>(*it);
		if (meshptr)
			SafeRelease(meshptr);
		else
			SafeRelease(*it);

		it = m_colliders.erase(it);
	}
	PX_RELEASE(m_body);
}

template<typename ColliderType, typename... Args>
ColliderType* RigidBody::AddCollider(Args&&... args)
{
	if (!std::is_base_of<Collider, ColliderType>::value)
	{
		throw std::runtime_error("ColliderType must inherit from Collider\n");
	}

	auto newCollider = new ColliderType(m_ownerGameObject, this, this, std::forward<Args>(args)...);
	newCollider->init();

	return newCollider;
}

template<typename ColliderType>
ColliderType* RigidBody::GetCollider(int index)
{
	//error check1
	if (!std::is_base_of<Collider, ColliderType>::value)
	{
		throw std::runtime_error("ColliderType must inherit from Collider\n");
	}

	//error check2
	if (index < 0 || index >= m_colliders.size())
	{
		throw std::runtime_error("Invalid collider index\n");
	}

	Collider* collider = m_colliders[index];

	//error check3
	if (dynamic_cast<ColliderType*>(collider))
	{
		return static_cast<ColliderType*>(collider);
	}
	else
	{
		throw std::runtime_error("GetCollider() at index: " + std::to_string(index) + " does not match ColliderType\n");
	}
}

template<typename ColliderType>
void RigidBody::RemoveCollider(ColliderType* colliderToRemove)
{
	if (!colliderToRemove)
	{
		throw std::runtime_error("Invalid collider pointer\n");
	}

	auto it = std::find_if(m_colliders.begin(), m_colliders.end(), [colliderToRemove](const Collider* collider) {
	 return collider == colliderToRemove; });

	if (it != m_colliders.end())
	{
		ColliderType* collider = dynamic_cast<ColliderType*>(*it);
		if (collider)
		{
			SafeRelease(collider);
			m_colliders.erase(it);
		}
		else
		{
			throw std::runtime_error("ColliderType does not match the type of the collider to remove\n");
		}
	}
	else
	{
		throw std::runtime_error("Collider not found in RigidBody colliders\n");
	}
}

void RigidBody::ApplyFlags()
{
	// 연속 충돌 감지 모드를 설정
	bool continousFlag = m_continuous && !isKinematic();
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, continousFlag);

	// 연속 충돌 모드 사용시 마찰을 사용합니다.
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, continousFlag);

	// SceneQuery에 현재 오브젝트의 위치가 아닌 Kinematic Traget Transform을 사용
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
}

void RigidBody::Attach(Collider* collider)
{
	if (!collider->m_shape)
		return;

	bool attach = m_body->attachShape(*(collider->m_shape));
	m_colliders.emplace_back(collider);

	UpdateMassAndInertia();
}

void RigidBody::Detach(Collider* collider)
{
	if (!collider->m_shape)
		return;

	m_body->detachShape(*(collider->m_shape));

	UpdateMassAndInertia();
}

PxRigidDynamic* RigidBody::GetBody()
{
	return m_body;
}

Collider* RigidBody::GetCollider(int index)
{
	if (index >= m_colliders.size())
	{
		return nullptr;
	}
	else
	{
		return m_colliders[index];
	}
}

PxVec3 RigidBody::GetPosition()
{
	return m_body->getGlobalPose().p;
}

void RigidBody::SetPosition(const Vec3 position, bool sleep)
{
	PxTransform t = m_body->getGlobalPose();
	t.p.x = position.x;
	t.p.y = position.y;
	t.p.z = position.z;

	m_body->setGlobalPose(t, sleep);
}

void RigidBody::SetRotation(const Quat rotation)
{
	PxTransform t = m_body->getGlobalPose();
	
	t.q.x = rotation.x;
	t.q.y = rotation.y;
	t.q.z = rotation.z;
	t.q.w = rotation.w;

	m_body->setGlobalPose(t);
}

void RigidBody::SetRotation(const float degrees, PhysicsAxis axis)
{
	// Get the current rotation of the rigid dynamic object
	PxTransform currentPose = m_body->getGlobalPose();

	// convert degree->radian
	float radians = PxPi * degrees / 180.0f;


	//quaternion representing rotation around axis
	PxQuat rotation(radians, PxVec3(0.f, 0.f, 0.f));
	switch (axis)
	{
		case PhysicsAxis::X:
		rotation = PxQuat(radians, PxVec3(1.f, 0.f, 0.f));
		break;
		case PhysicsAxis::Y:
		rotation = PxQuat(radians, PxVec3(0.f, 1.f, 0.f));
		break;
		case PhysicsAxis::Z:
		rotation = PxQuat(radians, PxVec3(0.f, 0.f, 1.f));
		break;
	}
	currentPose.q = rotation;
	currentPose.q.normalize();  // ensure the quaternion is normalized
	m_body->setGlobalPose(currentPose);
}

void RigidBody::SetTransfrom(const Transform* transform)
{
	PxTransform t = m_body->getGlobalPose();
	Vec3 position = transform->GetPosition();
	Quat rotation = transform->GetRotation();

	t.p.x = position.x;
	t.p.y = position.y;
	t.p.z = position.z;

	t.q.x = rotation.x;
	t.q.y = rotation.y;
	t.q.z = rotation.z;
	t.q.w = rotation.w;
}

PxVec3 RigidBody::GetVelocity() const
{
	return m_body->getLinearVelocity();
}

void RigidBody::SetVelocity(const PxVec3& velocity)
{
	if (isKinematic())
	{
		return;
	}

	m_body->setLinearVelocity(velocity);
}

float RigidBody::GetMass() const
{
	return m_body->getMass();
}

void RigidBody::SetMass(float value)
{
	m_body->setMass(value);
}

void RigidBody::UpdateMassAndInertia()
{
	PxRigidBodyExt::setMassAndUpdateInertia(*m_body, m_body->getMass());
}

bool RigidBody::IsRigidbodySleep() const
{
	return m_body->isSleeping();
}

void RigidBody::SetRigidbodySleep(bool value)
{
	if (isKinematic())
	{
		return;
	}

	if (!value) m_body->wakeUp();
	else		m_body->putToSleep();
}

float RigidBody::GetSleepThresholder() const
{
	return m_body->getSleepThreshold();
}

void RigidBody::SetSleepThresholder(float value)
{
	m_body->setSleepThreshold(value);
}

void RigidBody::SetRotationLockAxis(PhysicsAxis axes, bool value)
{
	PxU32 flag = (PxU32)axes << 3;
	m_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum(flag), value);
}

void RigidBody::SetKinematic(bool value)
{
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
}

void RigidBody::SetCCDFlag(bool value)
{
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, value);
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, value);
}

bool RigidBody::isKinematic() const
{
	return m_body->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
}
