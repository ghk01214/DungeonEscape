#include "pch.h"
#include "Collider.h"

Collider::Collider()
{
}

Collider::~Collider()
{
}

void Collider::Init(RigidBody* body)
{
	auto phys = PhysDevice::GetInstance()->GetPhysics();

#pragma region oldMatManagement
	//PxMaterial* newMat = phys->createMaterial(1.f, 1.f, 0.f);
	//newMat->setFrictionCombineMode(PxCombineMode::eMIN);
	//newMat->setRestitutionCombineMode(PxCombineMode::eMIN);			//apply material flags
	//ManageDuplicateMaterials(newMat);								//check for duplicates														
#pragma endregion

	m_material = phys->createMaterial(1.f, 1.f, 0.f);
	m_material->setFlag(physx::PxMaterialFlag::eDISABLE_FRICTION, false);
	m_material->setFlag(physx::PxMaterialFlag::eDISABLE_STRONG_FRICTION, false);

	//shape
	m_shape = phys->createShape(CreateGeometry().any(), *m_material, true);
	m_shape->userData = this;

	m_OwnerBody = body;
	m_OwnerBody->Attach(this);

	ApplyShapeFlags();
	ApplyTransform();
	ApplyLayer();

	SetFrictionCombineMode(PhysicsCombineMode::Min);
	SetRestitutionCombineMode(PhysicsCombineMode::Min);
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
	m_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, simulationFlag);
	m_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, triggerFlag);
	m_shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, queryFlag);
}

void Collider::ApplyTransform()
{
	physx::PxTransform t = GetRigidBody()->GetBody()->getGlobalPose();
	physx::PxTransform t2(physx::PxIdentity);
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

	physx::PxFilterData filter{};

	// 레이어 비트를 켭니다.
	filter.word0 = (1 << m_layerIndex);

	// 무시하지 않는 비트들을 켭니다.
	filter.word1 = unignoreBits;

	m_shape->setSimulationFilterData(filter);
}

bool Collider::CheckIfSameMaterial(physx::PxMaterial* mat1, physx::PxMaterial* mat2)
{
	if (mat1->getDynamicFriction() != mat2->getDynamicFriction()) return false;
	if (mat1->getStaticFriction() != mat2->getStaticFriction()) return false;
	if (mat1->getRestitution() != mat2->getRestitution()) return false;
	if (mat1->getFrictionCombineMode() != mat2->getFrictionCombineMode()) return false;
	if (mat1->getRestitutionCombineMode() != mat2->getRestitutionCombineMode()) return false;

	return true;
}

void Collider::ManageDuplicateMaterials(physx::PxMaterial*& newMat)
{
	bool found = false;
	for (size_t i = 0; i < m_materials.size(); i++)
	{
		if (CheckIfSameMaterial(m_materials[i], newMat))
		{
			newMat->release();
			newMat = m_materials[i];
			m_materialIndex = i;
			found = true;
			break;
		}
	}
	if (!found)
	{
		m_materialIndex = m_materials.size();
		m_materials.push_back(newMat);
	}
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
	physx::PxRigidActor* actor = m_shape->getActor();
	if (!actor)
		return nullptr;

	return (RigidBody*)actor->userData;
}

physx::PxShape* Collider::GetPxShape() const
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
	m_material->setFrictionCombineMode((physx::PxCombineMode::Enum)value);
}

PhysicsCombineMode Collider::GetRestitutionCombineMode() const
{
	return (PhysicsCombineMode)m_material->getRestitutionCombineMode();
}

void Collider::SetRestitutionCombineMode(PhysicsCombineMode value)
{
	m_material->setRestitutionCombineMode((physx::PxCombineMode::Enum)value);
}

void Collider::SetMaterialFlag(physx::PxMaterialFlag::Enum flag, bool value)
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
	}
}
