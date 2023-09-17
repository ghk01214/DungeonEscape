#include "pch.h"
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
#include "Transform.h"
#include "OverlapObject.h"
#include "MessageHandler.h"

using namespace physx;



MonsterAI::MonsterAI(Monster* monster) :
	m_monster(monster),
	m_detectRange(500000.f),
	m_targetPos_UpdateInterval(0.5f),
	m_checkRange{ 3000.f }
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
	if (!m_start)
		MonsterStart();

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

	if (players->GetGameObjects().size() < 2 && m_target == nullptr)		//1명밖에 없고 target이 없다
	{
		m_target = players->SelectRandomObject<Player>();
		return;
	}

	if (players->GetGameObjects().size() < 2 && m_target != nullptr)		//1명밖에 없고 target이 있다
	{
		return;
	}


	while (1)
	{
		auto newTarget = players->SelectRandomObject<Player>();				//랜덤 플레이어 선택
		if (newTarget == m_target)
			continue;
		else
		{
			m_target = newTarget;
			std::cout << "target변경" << std::endl;
		}
		return;

		//	Vec3 monsterPos = m_monster->GetControllerPosition();
		//if (newTarget)																//유효값 확인
		//{
		//	m_target = newTarget;
		//	m_targetPos = m_target->GetControllerPosition();
		//	if (Vec3::DistanceBetween(monsterPos, m_targetPos) < m_detectRange)		//몬스터 인식범위 확인
		//	{
		//		UpdateTargetPos();													//몬스터가 바라보는 방향 및 플레이어 위치 갱신
		//		return;
		//	}
		//}
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

void MonsterAI::Monstermove_ignoreWait()
{
	m_monster->GetController()->MonsterMove(TO_PX3(m_targetDir));
}

void MonsterAI::AddSkillSize(std::string scheduleName, GeometryType shape, Vec3 size, bool centerBox)
{
	auto skill = new MonsterSkill(scheduleName, shape, size, centerBox);
	m_skillSizeHolder.emplace_back(skill);
}

void MonsterAI::QuatUpdateForClient()
{
	UpdateTargetPos();
	Quat q = FROM_PXQUAT(GetRotation_For_Pattern(GetReverseXZDir()));

	if (q.x != 0 || q.z != 0)
		return;

	//전송.
	m_monster->m_rotation = q;

	game::TimerEvent ev{ ProtocolID::WR_MONSTER_QUAT_ACK };
	ev.objID = m_monster->m_id;

	MSG_HANDLER->PushSendMessage(ev);
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

MonsterSkill* MonsterAI::GetRequestedMonsterSkill(std::string schedule)
{
	for (auto& skill : m_skillSizeHolder)
	{
		if (skill->scheduleName == schedule)
			return skill;
	}

	return nullptr;
}

bool MonsterAI::SkillRangeCheck()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::string schedule = GetRequestedScheduleName();
	MonsterSkill* monsterSkill = GetRequestedMonsterSkill(schedule);
	PxGeometry* skillGeometry = GetRequestedSkillGeometry(schedule);
	if (!skillGeometry)
		return false;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

#pragma region 스킬 위치, 회전값 부여
	UpdateTargetPos();
	physx::PxVec3 xzDir = GetXZDir();
	m_monsterLook = FROM_PX3(xzDir);

	//위치, 회전값 적용
	physx::PxTransform trans;
	trans.p = TO_PX3(m_monster->GetControllerPosition());
	trans.q = GetRotation_For_Pattern(xzDir);

	//m_monster->GetTransform()->SetRotation(FROM_PXQUAT(trans.q));
	m_monster->GetTransform()->SetRotation(FROM_PXQUAT(GetRotation_For_Pattern(GetReverseXZDir())));

	float controllerRadius = m_monster->GetController()->GetCollider()->GetRadius();
#pragma endregion

	PxQueryFilterData filterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);
	constexpr PxU32 bufferSize = 10;  // Set this to whatever size you need
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer overlapBuffer(hitBuffer, bufferSize);

	switch (monsterSkill->skillGeometryType)
	{	//박스의 경우 전방에 배치
		case GeometryType::Box:
		{
			auto box = static_cast<PxBoxGeometry*>(skillGeometry);
			float extentZ = box->halfExtents.z;
			if (!monsterSkill->exception)					//centerBox가 true		중앙에 overlap 진행
			{												//centerBox가 false		보고 있는 방향으로 overlap 진행
				trans.p += TO_PX3(xzDir) * (extentZ + controllerRadius + 10);
			}
			query->Overlap(*box, trans, filterData, &overlapBuffer);
		}
		break;

		case GeometryType::Sphere:
		{
			auto sphere = static_cast<PxSphereGeometry*>(skillGeometry);
			query->Overlap(*sphere, trans, filterData, &overlapBuffer);
		}
		break;

		case GeometryType::Capsule:
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

float MonsterAI::GetXZDistance()
{
	if (!m_target)
		return -1;

	physx::PxVec3 targetPos = TO_PX3(m_target->GetControllerPosition());
	targetPos.y = 0;
	physx::PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	monsterPos.y = 0;

	physx::PxVec3 between = monsterPos - targetPos;
	return between.magnitude();
}

physx::PxVec3 MonsterAI::GetCounterEffectPosition()
{
	physx::PxVec3 currentPos = TO_PX3(m_monster->GetControllerPosition());
	physx::PxVec3 currentDir = TO_PX3(m_monsterLook);


	//float weeperDistance = 100.f;
	//float goelemDistance = 200.f;
	float monsterDistance = 0.f;

	//if (m_monster->GetName() == L"Golem")
	//	monsterDistance = goelemDistance;
	//else if (m_monster->GetName() == L"Weeper")
	//	monsterDistance = weeperDistance;

	currentPos += currentDir * monsterDistance;

	return currentPos;
}

std::vector<Player*> MonsterAI::SkillRangeCheck_OverlapObject(std::string scheduleName, OverlapObject* overlapObj)
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::vector<Player*> validPtrs;

	MonsterSkill* monsterSkill = GetRequestedMonsterSkill(scheduleName);
	PxGeometry* skillGeometry = GetRequestedSkillGeometry(scheduleName);
	if (!skillGeometry)
		return validPtrs;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

#pragma region 스킬 위치, 회전값 부여
	physx::PxVec3 xzDir = GetOldXZDir();

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
			if (!monsterSkill->exception)
			{
				float extentZ = box->halfExtents.z;
				trans.p += TO_PX3(xzDir) * (extentZ + controllerRadius + 20.f);
			}

			query->Overlap(*box, trans, filterData, &overlapBuffer);
		}
		break;

		case PxGeometryType::eSPHERE:
		{
			auto sphere = static_cast<PxSphereGeometry*>(skillGeometry);
			if (monsterSkill->exception)
			{
				physx::PxVec3 dir = TO_PX3(m_monster->GetAI()->m_targetDir);
				trans.p += dir * 200.f;				//몬스터가 바라보는 방향으로 200만큼 떨어진곳에서 처리
				trans.p.y -= m_monster->GetController()->GetCollider()->GetHalfHeight() + 10.f;		//몬스터 컨트롤러 발의 위치에서 터지도록
				overlapObj->UpdateOverlapPosition(trans.p);
			}
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

void MonsterAI::MonsterStart()
{
	Layer* layer = ObjectManager::GetInstance()->GetLayer(L"Layer_Player");
	if (!layer)
		return;									//레이어 유효값 체크

	auto players = layer->GetGameObjects();				//빈 레이어인지 체크
	if (players.size() < 1)
		return;

	for (auto& p : players)
	{
		auto player = dynamic_cast<Player*>(p);
		if (player)
		{
			physx::PxVec3 between = TO_PX3(m_monster->GetControllerPosition()) - TO_PX3(player->GetControllerPosition());
			if (between.magnitude() < m_checkRange)			//플레이어와의 거리 체크
			{
				m_start = true;
				SetAIWait(false);
				return;
			}
		}
	}

	return;
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

Player* MonsterAI::GetTarget()
{
	return m_target;
}

physx::PxVec3 MonsterAI::GetXZDir()
{
	if (!m_target)
		return PxVec3(0,0,0);

	Vec3 targetPos = m_target->GetControllerPosition();
	targetPos.y = 0;
	physx::PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	monsterPos.y = 0;

	PxVec3 dir = TO_PX3(targetPos) - monsterPos;
	dir.normalize();
	return dir;
}

physx::PxVec3 MonsterAI::GetReverseXZDir()
{
	if (!m_target)
		return PxVec3(0, 0, 0);

	Vec3 targetPos = m_target->GetControllerPosition();
	targetPos.y = 0;
	physx::PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	monsterPos.y = 0;

	PxVec3 dir = monsterPos - TO_PX3(targetPos);
	dir.normalize();
	return dir;
}

physx::PxVec3 MonsterAI::GetOldXZDir()
{
	if (!m_target)
		return PxVec3(0, 0, 0);

	Vec3 targetPos = m_targetPos;
	targetPos.y = 0;
	physx::PxVec3 monsterPos = TO_PX3(m_monster->GetControllerPosition());
	monsterPos.y = 0;

	PxVec3 dir = TO_PX3(targetPos) - monsterPos;
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

void MonsterAI::BossPatternUIStart(server::TRIGGER_INTERACTION_TYPE type)
{
	game::TimerEvent ev{ ProtocolID::WR_TRIGGER_INTERACTION_ACK };
	ev.objID = m_monster->GetID();
	ev.integer = magic_enum::enum_integer(type);

	MSG_HANDLER->PushSendMessage(ev);
}
