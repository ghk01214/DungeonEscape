#include "pch.h"
#include "MonsterAI.h"
#include "Monster.h"
#include "Player.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "CustomController.h"
#include "PhysDevice.h"
#include "PhysQuery.h"
#include "RigidBody.h"

using namespace physx;

MonsterAI::MonsterAI(Monster* monster) :
	m_monster(monster),
	m_detectRange{ 5000000.f },
	m_targetPos_UpdateInterval{ 0.5f }
{
}

MonsterAI::~MonsterAI()
{
}

void MonsterAI::Init()
{
	SetRandomTarget();
	m_scheduler.emplace_back(SCHEDULE::WEEPER_CAST1);			//임시
}

void MonsterAI::Update()
{
	SchedulerUpdate();
}

void MonsterAI::Late_Update()
{
}

void MonsterAI::Release()
{
	for (auto& pair : m_skillSizeHolder)
	{
		delete pair.second;
	}
}

void MonsterAI::SchedulerUpdate()
{
	if (!m_scheduler.empty())
		return;

	if (m_scheduler[0] == SCHEDULE::WEEPER_CAST1)
		SkillRangeCheck();
}

void MonsterAI::SetRandomTarget()
{
	Layer* players = ObjectManager::GetInstance()->GetLayer(L"Player");
	Vec3 monsterPos = m_monster->GetControllerPosition();
	while (1)
	{
		auto newTarget = players->SelectRandomObject<Player>();						//랜덤 플레이어 선택
		if (m_target == newTarget)
			continue;

		if (m_target)																//유효값 확인
		{
			m_targetPos = m_target->GetControllerPosition();
			if (Vec3::DistanceBetween(monsterPos, m_targetPos) < m_detectRange)		//몬스터 인식범위 확인
			{
				UpdateTargetPos();													//몬스터가 바라보는 방향 및 플레이어 위치 갱신
				return;
			}
		}
	}
}

void MonsterAI::UpdateTargetPos()
{
	// 몬스터가 바라보는 방향 및 플레이어 위치 갱신
	if (!m_target)
		return;

	m_targetPos = m_target->GetControllerPosition();
	PxVec3 dir = TO_PX3(m_monster->GetControllerPosition()) - TO_PX3(m_targetPos);
	dir.normalize();
	m_targetDir = FROM_PX3(dir);
}

void MonsterAI::AddSkillSize(GeometryType type, Vec3 size, const std::wstring& skillTag)
{
	PxGeometry* skillGeometry;
	PxVec3 pxSize = TO_PX3(size);

	if (type == GeometryType::Box)
	{
		auto box = new PxBoxGeometry(pxSize);
		if (!box->isValid())
			return;
		skillGeometry = box;
	}
	else if (type == GeometryType::Sphere)
	{
		auto sphere = new PxSphereGeometry(pxSize.x);
		if (!sphere->isValid())
			return;
		skillGeometry = sphere;
	}
	else if (type == GeometryType::Capsule)
	{
		auto capsule = new PxCapsuleGeometry(pxSize.x, pxSize.y);
		if (!capsule->isValid())
			return;
		skillGeometry = capsule;
	}


	auto pair = std::make_pair(skillTag, skillGeometry);
	m_skillSizeHolder;
	m_skillSizeHolder.emplace(pair);
}

bool MonsterAI::SkillRangeCheck()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	PxGeometry* skillGeometry = GetSkillGeometry();
	if (!skillGeometry)
		return false;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

#pragma region 스킬 위치, 회전값 부여
	PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	PxTransform trans(PxIdentity);										
	
	PxVec3 up(0, 1, 0);
	PxVec3 right = up.cross(TO_PX3(m_targetDir)).getNormalized();
	PxVec3 realUp = TO_PX3(m_targetDir).cross(right);
	PxMat33 m(right, realUp, TO_PX3(m_targetDir));
	PxQuat monsterRot(m);

	trans.p = monsterPos;
	trans.q = monsterRot;
#pragma endregion


	PxQueryFilterData filterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);
	constexpr PxU32 bufferSize = 10;  // Set this to whatever size you need
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer overlapBuffer(hitBuffer, bufferSize);

	switch (skillGeometry->getType())
	{
		case PxGeometryType::eBOX:
		{
			//query->Raycast(hit, ray, 1 << static_cast<uint8_t>(PhysicsLayers::Map), PhysicsQueryType::Collider, m_body)
			auto box = static_cast<PxBoxGeometry*>(skillGeometry);
			float extentZ = box->halfExtents.z;
			trans.p += TO_PX3(m_targetDir) * extentZ;

			query->Overlap(*box, trans, filterData, &overlapBuffer);
		}
		break;
	}


	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxRigidActor* actor = hit.actor;
		auto body = static_cast<RigidBody*>(actor->userData);
		
		std::cout << "Name : " << body->GetName() << std::endl;
	}
	
}

physx::PxGeometry* MonsterAI::GetSkillGeometry()
{
	if (m_scheduler[0] == SCHEDULE::WEEPER_CAST1)
	{
		auto iter = m_skillSizeHolder.find(L"WEEPER_CAST1");
		if (iter != m_skillSizeHolder.end())
		{
			auto skillGeometry = iter->second;
			auto device = PhysDevice::GetInstance();
		}
		return nullptr;			//스킬 범위 탐색 실패. 오류.
	}
}


