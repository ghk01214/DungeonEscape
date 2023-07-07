#include "pch.h"
#include "MonsterAI.h"
#include "Monster.h"
#include "MonsterSkill.h"
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
	m_detectRange(500000.f),
	m_targetPos_UpdateInterval(0.5f)
{
}

MonsterAI::~MonsterAI()
{
}

void MonsterAI::Init()
{

}

void MonsterAI::Update(float timeDelta)
{
	if (m_AIWait)
		return;

	if (IsEmptySchedule())
	{
		SetRandomTarget();
		FillSchedule();
	}

	else
	{
		ExecuteSchedule(timeDelta);
	}
}

void MonsterAI::LateUpdate(float timeDelta)
{
}

void MonsterAI::Release()
{
	for (auto& skill : m_skillSizeHolder)
	{
		skill->Release();
		SafeRelease(skill);
	}
	m_skillSizeHolder.clear();
}

void MonsterAI::SetRandomTarget()
{
	Layer* players = ObjectManager::GetInstance()->GetLayer(L"Layer_Player");
	if (!players)
		return;

	Vec3 monsterPos = m_monster->GetControllerPosition();
	while (1)
	{
		auto newTarget = players->SelectRandomObject<Player>();						//랜덤 플레이어 선택
		if (m_target == newTarget && players->GetGameObjects().size() > 1)			//남은 플레이어가 1명보다 많고, 랜덤선정이 중복플레이어를 타겟으로 삼으면 continue 
			continue;

		if (newTarget)																//유효값 확인
		{
			m_target = newTarget;
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
	PxVec3 dir = TO_PX3(m_targetPos) - TO_PX3(m_monster->GetControllerPosition());
	dir.normalize();
	m_targetDir = FROM_PX3(dir);
}

void MonsterAI::AddSkillSize(std::string scheduleName, GeometryType shape, Vec3 size)
{
	auto skill = new MonsterSkill(scheduleName, shape, size);
	m_skillSizeHolder.emplace_back(skill);
}

physx::PxGeometry* MonsterAI::GetRequestedSkillGeometry(std::string schedule)
{
	for (auto& skill : m_skillSizeHolder)
	{
		if (skill->scheduleName == schedule)
			return skill->skillGeometry;
	}

	return nullptr;
}

bool MonsterAI::SkillRangeCheck()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::string schedule = GetRequestedScheduleName();
	PxGeometry* skillGeometry = GetRequestedSkillGeometry(schedule);
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
	{	//박스의 경우 전방에 배치
		case PxGeometryType::eBOX:
		{
			auto box = static_cast<PxBoxGeometry*>(skillGeometry);
			float extentZ = box->halfExtents.z;
			trans.p += TO_PX3(m_targetDir) * extentZ;

			query->Overlap(*box, trans, filterData, &overlapBuffer);
		}
		break;

		case PxGeometryType::eSPHERE:
		{
			auto sphere = static_cast<PxSphereGeometry*>(skillGeometry);
			query->Overlap(*sphere, trans, filterData, &overlapBuffer);
		}
		break;

		case PxGeometryType::eCAPSULE:
		{
			auto capsule = static_cast<PxCapsuleGeometry*>(skillGeometry);
			query->Overlap(*capsule, trans, filterData, &overlapBuffer);
		}
		break;
	}


	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxRigidActor* actor = hit.actor;
		auto body = static_cast<RigidBody*>(actor->userData);
		std::string name = body->GetName();
		if (name == "Player")
		{
			std::cout << "skillRangecheck" << std::endl;
			return true;
		}
	}
	return false;
}

void MonsterAI::SetAIWait(bool value)
{
	m_AIWait = value;
}

bool MonsterAI::GetAIWait()
{
	return m_AIWait;
}




