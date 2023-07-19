﻿#include "pch.h"
#include "MonsterAI.h"
#include "Monster.h"
#include "MonsterSkill.h"
#include "Player.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "CustomController.h"
#include "CapsuleCollider.h"
#include "PhysDevice.h"
#include "PhysQuery.h"
#include "RigidBody.h"
#include "OverlapObject.h"

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

	if (players->GetGameObjects().size() < 1)
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

void MonsterAI::Monstermove()
{
	if (m_AIWait)
		return;

	m_monster->GetController()->MonsterMove(TO_PX3(m_targetDir));
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
	UpdateTargetPos();
	physx::PxVec3 xzDir = GetXZDir();

	//위치, 회전값 적용
	physx::PxTransform trans;
	trans.p = TO_PX3(m_monster->GetControllerPosition());
	trans.q = GetRotation_For_Pattern(xzDir);

	float controllerRadius = m_monster->GetController()->GetCollider()->GetRadius();
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
			trans.p += TO_PX3(xzDir) * (extentZ + controllerRadius);

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

std::vector<Player*> MonsterAI::SkillRangeCheck_OverlapObject(std::string scheduleName)
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::vector<Player*> validPtrs;

	PxGeometry* skillGeometry = GetRequestedSkillGeometry(scheduleName);
	if (!skillGeometry)
		return validPtrs;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

#pragma region 스킬 위치, 회전값 부여
	physx::PxVec3 xzDir = GetXZDir();

	//위치, 회전값 적용
	physx::PxTransform trans;
	trans.p = TO_PX3(m_monster->GetControllerPosition());
	trans.q = GetRotation_For_Pattern(xzDir);

	float controllerRadius = m_monster->GetController()->GetCollider()->GetRadius();
#pragma endregion

#pragma region QueryFilter, Buffer 초기화 및 Geometry에 맞춘 Overlap 진행
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
			trans.p += TO_PX3(xzDir) * (extentZ + controllerRadius + 20.f);

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

#pragma endregion 

	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxShape* shape = hit.shape;
		PxRigidActor* actor = shape->getActor();

		auto body = static_cast<RigidBody*>(actor->userData);

		std::string name = body->GetName();
		if (name == "Player")
		{
			auto player = dynamic_cast<Player*>(body->GetOwnerObject());
			validPtrs.emplace_back(player);
			int size = validPtrs.size();
		}
	}

	return validPtrs;
}

Monster* MonsterAI::GetMonster()
{
	return m_monster;
}

void MonsterAI::SetAIWait(bool value)
{
	m_AIWait = value;
}

bool MonsterAI::GetAIWait()
{
	return m_AIWait;
}

Vec3 MonsterAI::GetTargetPosition()
{
	if (m_target)
		return m_target->GetControllerPosition();
	else
		return Vec3(0, 1000, 0);
}

Vec3 MonsterAI::GetTargetDir()
{
	return m_targetDir;
}

physx::PxVec3 MonsterAI::GetXZDir()
{
	if (!m_target)
		return PxVec3(0,0,0);

	m_targetPos = m_target->GetControllerPosition();
	m_targetPos.y = 0;
	physx::PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	monsterPos.y = 0;

	PxVec3 dir = TO_PX3(m_targetPos) - monsterPos;
	dir.normalize();
	return dir;
}

physx::PxQuat MonsterAI::GetRotation_For_Pattern(physx::PxVec3 xzDir)
{
	physx::PxVec3 initialDir(0, 0, 1);
	physx::PxVec3 rotAxis = initialDir.cross(xzDir).getNormalized();
	physx::PxReal rotAngle = physx::PxAtan2(physx::PxSqrt(1 - physx::PxPow(xzDir.dot(initialDir), 2)), xzDir.dot(initialDir));
	physx::PxQuat rotation(rotAngle, rotAxis);

	return rotation;
}




