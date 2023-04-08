#include "pch.h"
#include "RigidBody.h"

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"

void RigidBody::Init(ColliderShape shape)
{
	auto device = PhysDevice::GetInstance();

	physx::PxTransform pose(physx::PxIdentity);

	m_body = device->GetPhysics()->createRigidDynamic(pose);
	m_body->setMass(10);
	m_body->userData = this;

	ApplyFlags();

	device->GetScene()->addActor(*m_body);

	switch (shape)
	{
		case ColliderShape::COLLIDER_BOX:
		{
			BoxCollider* box = new BoxCollider;
			box->Init(this);
			m_colliders.emplace_back(box);
			break;
		}
		case ColliderShape::COLLIDER_SPHERE:
		{
			SphereCollider* sphere = new SphereCollider;
			sphere->Init(this);
			m_colliders.emplace_back(sphere);
			break;
		}
		case ColliderShape::COLLIDER_CAPSULE:
		{
			CapsuleCollider* capsule = new CapsuleCollider;
			capsule->Init(this);
			m_colliders.emplace_back(capsule);
			break;
		}
		case ColliderShape::COLLIDER_MESH:
		{
			// body needs to be kinematic to apply meshGeometry
			MeshCollider* mesh = new MeshCollider;
			mesh->Init(this);

			m_colliders.emplace_back(mesh);
			break;
		}
	}
}

void RigidBody::ApplyFlags()
{
	// 연속 충돌 감지 모드를 설정
	bool continousFlag = m_continuous && !isKinematic();
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, continousFlag);

	// 연속 충돌 모드 사용시 마찰을 사용합니다.
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, continousFlag);

	// SceneQuery에 현재 오브젝트의 위치가 아닌 Kinematic Traget Transform을 사용
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
}

void RigidBody::AttachAll()
{
	DetachAll();

	for (auto collider : m_colliders)
	{
		Attach(collider);
	}
}

void RigidBody::DetachAll()
{
	physx::PxU32 nb = m_body->getNbShapes();
	physx::PxShape** shapes = new physx::PxShape * [nb];
	m_body->getShapes(shapes, sizeof(physx::PxShape*) * nb);

	for (physx::PxU32 i = 0; i < nb; ++i)
	{
		m_body->detachShape(*shapes[i]);
	}

	SafeDeleteArray(shapes);
}

void RigidBody::Attach(Collider* collider)
{
	if (!collider->m_shape)
		return;

	bool attach = m_body->attachShape(*(collider->m_shape));
	UpdateMassAndInertia();
}

void RigidBody::Detach(Collider* collider)
{
	if (!collider->m_shape)
		return;

	m_body->detachShape(*(collider->m_shape));
	UpdateMassAndInertia();
}

physx::PxRigidDynamic* RigidBody::GetBody()
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

physx::PxVec3 RigidBody::GetPosition()
{
	return m_body->getGlobalPose().p;
}

void RigidBody::SetPosition(const float x, const float y, const float z, bool sleep)
{
	physx::PxTransform t = m_body->getGlobalPose();
	t.p.x = x;
	t.p.y = y;
	t.p.z = z;

	m_body->setGlobalPose(t, sleep);
}

void RigidBody::SetRotation(float degrees, PhysicsAxis axis)
{
	// Get the current rotation of the rigid dynamic object
	physx::PxTransform currentPose = m_body->getGlobalPose();

	// convert degree->radian
	float radians = physx::PxPi * degrees / 180.0f;


	//quaternion representing rotation around axis
	physx::PxQuat rotation(radians, physx::PxVec3(0.f, 0.f, 0.f));
	switch (axis)
	{
		case PhysicsAxis::X:
		rotation = physx::PxQuat(radians, physx::PxVec3(1.f, 0.f, 0.f));
		break;
		case PhysicsAxis::Y:
		rotation = physx::PxQuat(radians, physx::PxVec3(0.f, 1.f, 0.f));
		break;
		case PhysicsAxis::Z:
		rotation = physx::PxQuat(radians, physx::PxVec3(0.f, 0.f, 1.f));
		break;
	}
	currentPose.q = rotation;
	currentPose.q.normalize();  // ensure the quaternion is normalized
	m_body->setGlobalPose(currentPose);

#pragma region 혹시 몰라서 남겨둔다
	//Get the current rotation of the rigid dynamic object
	//PxTransform currentPose = m_body->getGlobalPose();

	//static float angle = 90.f;
	////angle += 0.1f;

	//float radian = PxPi * angle / 180.0f;
	//currentPose.q = PxQuat(radian, PxVec3(0.f,0.f,1.f));
	//m_body->setGlobalPose(currentPose);
#pragma endregion
}



physx::PxVec3 RigidBody::GetVelocity() const
{
	return m_body->getLinearVelocity();
}

void RigidBody::SetVelocity(const physx::PxVec3& velocity)
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
	physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_body, m_body->getMass());
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
	physx::PxU32 flag = (physx::PxU32)axes << 3;
	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum(flag), value);
}

void RigidBody::SetKinematic(bool value)
{
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}

void RigidBody::SetCCDFlag(bool value)
{
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, value);
	m_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, value);
}

bool RigidBody::isKinematic() const
{
	return m_body->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
}
