#include "pch.h"
#include "Collider.h"
#include "PhysDevice.h"
#include "RigidBody.h"

using namespace physx;

Collider::Collider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body)
	: Component(ownerGameObject, ownerComponent), m_attachedRigidBody(body), m_shape(nullptr)
{

}

Collider::~Collider()
{
}

void Collider::Init()
{
	auto phys = PhysDevice::GetInstance()->GetPhysics();
	auto cooking = PhysDevice::GetInstance()->GetCooking();

	m_material = phys->createMaterial(1.f, 1.f, 0.f);
	m_material->setFlag(PxMaterialFlag::eDISABLE_FRICTION, false);
	m_material->setFlag(PxMaterialFlag::eDISABLE_STRONG_FRICTION, false);

	//shape
	m_shape = phys->createShape(CreateGeometry().any(), *m_material, true);
	m_shape->userData = this;

	m_attachedRigidBody->Attach(this);

	ApplyShapeFlags();
	//ApplyTransform();
	ApplyLayer();

	SetFrictionCombineMode(PhysicsCombineMode::Min);
	SetRestitutionCombineMode(PhysicsCombineMode::Min);
}

void Collider::Release()
{
	m_attachedRigidBody->Detach(this);
	//멤버 함수 정리
	PX_RELEASE(m_shape);
	PX_RELEASE(m_material);
	ClearCollisionInfo();
}


void Collider::ApplyShapeFlags()
{
	// 트리거 플래그: 트리거 모드
	// 시뮬레이션 플래그: 트리거 플래그가 꺼져 있으면 활성화
	// 쿼리 플래그: 컴포넌트가 활성화 상태라면 언제나 켜져 있다.

	bool triggerFlag = false;
	bool simulationFlag = !triggerFlag;
	bool queryFlag = true;

	// eTRIGGER_SHAPE 플래그를 활성화 하기 전에는 반드시 eSIMULATION_SHAPE 플래그가 비활성화 상태여야 합니다.
	m_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, simulationFlag);
	m_shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, triggerFlag);
	m_shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, queryFlag);
}

void Collider::ApplyTransform()
{
	PxTransform t = GetRigidBody()->GetBody()->getGlobalPose();
	PxTransform t2(PxIdentity);
	m_shape->setLocalPose(t);
}

void Collider::ApplyScale()
{
	ResetShape();
}

void Collider::ApplyLayer()
{
	auto device = PhysDevice::GetInstance();

	// 무시할 레이어를 제외한 모든 비트를 켭니다.
	uint32_t unignoreBits = ~m_ignoreLayerBits;

	PxFilterData filter{};

	// 레이어 비트를 켭니다.
	filter.word0 = (1 << m_layerIndex);

	// 무시하지 않는 비트들을 켭니다.
	filter.word1 = unignoreBits;

	m_shape->setSimulationFilterData(filter);
}

void Collider::ResetShape()
{
	m_shape->setGeometry(CreateGeometry().any());

	RigidBody* body = GetRigidBody();
	if (body)
	{
		body->UpdateMassAndInertia();
	}
}

uint8_t Collider::GetLayerIndex() const
{
	return m_layerIndex;
}

void Collider::SetLayerIndex(uint8_t layerIndex)
{
	assert(layerIndex < 32);

	m_layerIndex = layerIndex;

	ApplyLayer();
}

RigidBody* Collider::GetRigidBody() const
{
	PxRigidActor* actor = m_shape->getActor();
	if (!actor)
		return nullptr;

	return (RigidBody*)actor->userData;
}

PxShape* Collider::GetPxShape() const
{
	return m_shape;
}

float Collider::GetFriction(float value) const
{
	return m_material->getDynamicFriction();
}

void Collider::SetFriction(float value)
{
	m_material->setStaticFriction(value);
	m_material->setDynamicFriction(value);
}

PhysicsCombineMode Collider::GetFrictionCombineMode() const
{
	return (PhysicsCombineMode)m_material->getFrictionCombineMode();
}

void Collider::SetFrictionCombineMode(PhysicsCombineMode value)
{
	m_material->setFrictionCombineMode((PxCombineMode::Enum)value);
}

PhysicsCombineMode Collider::GetRestitutionCombineMode() const
{
	return (PhysicsCombineMode)m_material->getRestitutionCombineMode();
}

void Collider::SetRestitutionCombineMode(PhysicsCombineMode value)
{
	m_material->setRestitutionCombineMode((PxCombineMode::Enum)value);
}

void Collider::SetMaterialFlag(PxMaterialFlag::Enum flag, bool value)
{
	m_material->setFlag(flag, value);
}

void Collider::CollectCollisionInfo(CollisionInfoType type, std::shared_ptr<CollisionPairInfo> info)
{
	switch (type)
	{
		case CollisionInfoType::Enter:
		m_CollisionEnter.emplace_back(info);
		break;
		case CollisionInfoType::Stay:
		m_CollisionStay.emplace_back(info);
		break;
		case CollisionInfoType::Exit:
		m_CollisionExit.emplace_back(info);
		break;
	}
}

void Collider::ClearCollisionInfo()
{
	m_CollisionEnter.clear();
	m_CollisionStay.clear();
	m_CollisionExit.clear();
}

const std::vector<std::shared_ptr<CollisionPairInfo>>& Collider::GetCollisionInfo(CollisionInfoType type) const
{
	switch (type)
	{
		case CollisionInfoType::Enter:
		return m_CollisionEnter;
		case CollisionInfoType::Stay:
		return m_CollisionStay;
		case CollisionInfoType::Exit:
		return m_CollisionExit;
		default:
		throw std::runtime_error("Invalid CollisionInfoType value");
	}
}
