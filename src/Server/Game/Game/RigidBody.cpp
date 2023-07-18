#include "pch.h"
#include "RigidBody.h"
#include "PhysDevice.h"
#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshCollider.h"


using namespace physx;

RigidBody::RigidBody(GameObject* ownerGameObject, Component* ownerComponent)
	: Component(ownerGameObject, ownerComponent), m_body(nullptr)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::Init()
{
	auto device = PhysDevice::GetInstance();

	auto trans = m_ownerGameObject->GetTransform();
	auto pos = trans->GetPosition();
	PxTransform pose(PxIdentity);

	m_body = device->GetPhysics()->createRigidDynamic(pose);
	m_body->setMass(10);
	m_body->userData = this;
	
	std::string str = "debug";
	char* name = new char[str.length() + 1]; // 문자열의 길이 + 널 종료 문자
	std::copy(str.begin(), str.end(), name); // 문자열 복사
	name[str.length()] = '\0'; // 널 종료 문자 추가

	m_body->setName(name);

	ApplyFlags();
	SetTransform(trans);

	device->GetScene()->addActor(*m_body);

	//원래 여기서 collider 생성, init, 부착까지 완료
}

void RigidBody::Release()
{
	auto device = PhysDevice::GetInstance();

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

	device->GetScene()->removeActor(*m_body);

	PX_RELEASE(m_body);
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

void RigidBody::ClearCollidersCollisionInfo()
{
	//GetCollider(0)->ClearCollisionInfo();
	for (auto& collider : m_colliders)
	{
		collider->ClearCollisionInfo();
	}
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

physx::PxTransform RigidBody::GetGlobalPose()
{
	return m_body->getGlobalPose();
}

PxVec3 RigidBody::GetPosition()
{
	return m_body->getGlobalPose().p;
}

physx::PxQuat RigidBody::GetRotation()
{
	return m_body->getGlobalPose().q;
}

void RigidBody::SetPosition(const Vec3 position, bool wake)
{
	PxTransform t = m_body->getGlobalPose();
	t.p.x = position.x;
	t.p.y = position.y;
	t.p.z = position.z;

	m_body->setGlobalPose(t, wake);
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
		default:
		throw std::runtime_error("Invalid PhysicsAxis value");
	}
	currentPose.q = rotation;
	currentPose.q.normalize();  // ensure the quaternion is normalized
	m_body->setGlobalPose(currentPose);
}

void RigidBody::SetTransform(const Transform* transform)
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

	m_body->setGlobalPose(t);
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
	UpdateMassAndInertia();
}

void RigidBody::AddForce(ForceMode mode, const PxVec3& forceVector)
{
	if (isKinematic())
	{
		return;
	}

	m_body->addForce(forceVector, (PxForceMode::Enum)mode);
}

void RigidBody::AddTorque(ForceMode mode, const physx::PxVec3& forceVector)
{
	if (isKinematic())
	{
		return;
	}

	m_body->addTorque(forceVector, (PxForceMode::Enum)mode);
}

void RigidBody::AddRandomTorque(ForceMode mode, float strength)
{
	static std::random_device rd;  // 랜덤 시드 생성
	static std::mt19937 gen(rd()); // Mersenne twister 엔진 초기화
	static std::uniform_real_distribution<> dis(-1.0, 1.0); // -1.0에서 1.0까지의 균등 분포 생성

	if (isKinematic())
	{
		return;
	}

	PxVec3 randomTorque = PxVec3(dis(gen), dis(gen), dis(gen));
	randomTorque *= strength;

	m_body->addTorque(randomTorque, (PxForceMode::Enum)mode);
}

void RigidBody::AddRandomTorque_PlanetStyle(ForceMode mode, float strength)
{
	static std::uniform_real_distribution<> dis(0.5, 1.0);
	static std::uniform_real_distribution<> disXZ(-1.0, 1.0); // -1.0에서 1.0까지의 균등 분포 생성
	static std::uniform_real_distribution<> disY(-0.2, 0.2); // 예를 들어, -0.2에서 0.2까지의 균등 분포 생성

	if (isKinematic())
	{
		return;
	}

	PxVec3 randomTorque = PxVec3(disXZ(dre), disY(dre), disXZ(dre));
	randomTorque *= strength;

	m_body->addTorque(randomTorque, (PxForceMode::Enum)mode);
}

void RigidBody::AddRandomTorque_Vertical(ForceMode mode, float strength)
{
	static std::uniform_real_distribution<> disY(-1.0, 1.0); // 예를 들어, -0.2에서 0.2까지의 균등 분포 생성

	if (isKinematic())
	{
		return;
	}

	PxVec3 randomTorque = PxVec3(0.f, disY(dre), 0);
	randomTorque *= strength;

	m_body->addTorque(randomTorque, (PxForceMode::Enum)mode);
}

void RigidBody::SetAngularDamping(float value)
{
	if (m_body)
		m_body->setAngularDamping(value);
}

void RigidBody::SetLinearDamping(float value)
{
	if (m_body)
		m_body->setLinearDamping(value);
}



void RigidBody::UpdateMassAndInertia()
{
	PxRigidBodyExt::setMassAndUpdateInertia(*m_body, m_body->getMass());
}

bool RigidBody::IsRigidbodySleep() const
{
	return m_body->isSleeping();
}

void RigidBody::SetRigidBodySleep(bool value)
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

std::string RigidBody::GetName()
{
	return m_name;
}

void RigidBody::SetName(std::string& name)
{
	m_name = name;
}

void RigidBody::ExcludeFromSimulation(bool value)
{
	m_body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, value);
}

bool RigidBody::IsExcludedFromSimulation()
{
	return m_body->getActorFlags().isSet(PxActorFlag::eDISABLE_SIMULATION);
}

