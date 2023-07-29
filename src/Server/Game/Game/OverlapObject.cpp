#include "pch.h"
#include "OverlapObject.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "Player.h"
#include "RigidBody.h"
#include "Collider.h"
#include "CustomController.h"
#include "EventHandler.h"
#include "RaycastHit.h"
#include "PhysDevice.h"
#include "physx_utils.h"

using namespace physx;
using namespace std;

OverlapObject::OverlapObject(MonsterAI* monsterAI)
	: m_monsterAI(monsterAI)
{
}

OverlapObject::~OverlapObject()
{
}

void OverlapObject::Update()
{
	Active();
}

void OverlapObject::Release()
{
	Deactivate();
	m_monsterAI = nullptr;
}

void OverlapObject::Activate(std::string scheduleName)
{
	m_currentScheduleName = scheduleName;
	m_active  = true;
}

void OverlapObject::Deactivate()
{
	m_duplicates.clear();
	m_active = false;
}

void OverlapObject::Active()
{
	if (!m_active)
		return;

	std::vector<Player*> validPlayers = m_monsterAI->SkillRangeCheck_OverlapObject(m_currentScheduleName, this);

	for (auto& player : validPlayers)
	{
		bool duplicate = IsPlayerDuplicate(player);
		if (duplicate)
			continue;
		else
		{
			bool applied = ApplyMonsterSkillToPlayer(player);		//spell은 raycast후 실패하면 return false
			if(applied)
				m_duplicates.emplace_back(player);
		}
	}
}

bool OverlapObject::IsPlayerDuplicate(Player* player)
{
	if (std::find(m_duplicates.begin(), m_duplicates.end(), player) == m_duplicates.end())
		return false;

	return true;
}

bool OverlapObject::ApplyMonsterSkillToPlayer(Player* player)
{
	auto playerController = player->GetController();
	auto playerBody = playerController->GetBody();
	PxVec3 xzDir = m_monsterAI->GetXZDir();

	PxVec3 monsterPos = TO_PX3(m_monsterAI->GetMonster()->GetControllerPosition());
	PxVec3 playerPos = playerBody->GetPosition();
	PxVec3 difference = monsterPos - playerPos;
	float distance = difference.magnitude();			//몬스터 - 플레이어 거리

	//AddSkillSize로 ai에 추가된 MonsterSkill의 이름을 기준으로 작성
	if (m_currentScheduleName == "ATTACK1")
	{
		float dragPower = 150.f;
		if (distance < 600)
			dragPower = 10.f;
		if (distance < 300)
			dragPower = 2.f;
		if (distance < 200)
			dragPower = 0.1f;
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->SetVelocity(PxVec3(0));
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 1000.f);
		//playerBody->AddForce(ForceMode::Impulse, -xzDir * dragPower);

		return true;
	}

	else if (m_currentScheduleName == "ATTACK2")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 800.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 200.f);
		EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK_DAMAGE_APPLY", 0.1f, player);			//continous. 땅에 닿으면 피격 애니메이션 재생

		return true;
	}

	else if (m_currentScheduleName == "ATTACK3")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 300.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, -1, 0) * 2000.f);
		EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK3_STUN_APPLY", 0.1f, player);			//continous. 땅에 닿으면 스턴 + 스턴해제 5초후 명령을 내린다.

		return true;
	}

	else if (m_currentScheduleName == "ATTACK4")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 200.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 200.f);
		EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK_DAMAGE_APPLY", 0.1f, player);			//continous. 땅에 닿으면 피격 애니메이션 재생

		return true;
	}

	else if (m_currentScheduleName == "ROAR")
	{
		player->SetStun(true);
		EventHandler::GetInstance()->AddEvent("PLAYER_STUN_OFF", 4.f, player);			//5초 스턴

		return true;
	}

	else if (m_currentScheduleName == "RUN")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 1000.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 250.f);
		EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK_DAMAGE_APPLY", 0.f, player);			//continous. 땅에 닿으면 피격 애니메이션 재생

		return true;
	}

	else if (m_currentScheduleName == "SPELL")
	{
		if (RaycastPlayer(player))
		{
			cout << "spell 피격" << endl;
			playerBody->SetVelocity(PxVec3(0));
			playerController->BounceFromAttack();

			physx::PxVec3 knockbackDir = playerBody->GetPosition() - m_currentOverlapPos;
			knockbackDir.normalize();

			playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0,1,0) * 400.f);
			playerBody->AddForce(ForceMode::Impulse, knockbackDir * 600.f);
			EventHandler::GetInstance()->AddEvent("GOLEM_ATTACK_DAMAGE_APPLY", 0.1f, player);			//continous. 땅에 닿으면 피격 애니메이션 재생
			return true;
		}
	}

	return false;		//spell의 raycast 실패시 false를 리턴한다
}

bool OverlapObject::RaycastPlayer(Player* player)
{
	auto device = PhysDevice::GetInstance();
	auto query = device->GetQuery();

	RaycastHit hit;
	PhysicsRay ray;
	physx::PxVec3 dir = TO_PX3(player->GetControllerPosition()) - m_currentOverlapPos;
	dir.normalize();

	ray.direction = dir;
	ray.distance = 1500.f;						//raycast 측정 거리. GolemAI::Init()에서 SPELL사이즈와 값과 일치해야한다.
	ray.point = m_currentOverlapPos;
																											//몬스터 본인의 강체는 무시
	if (query->Raycast(hit, ray, 1 << static_cast<uint8_t>(PhysicsLayers::PLAYER), PhysicsQueryType::All, m_monsterAI->GetMonster()->GetController()->GetBody()))
	{
		string name = hit.collider->GetRigidBody()->GetName();
		if(name == "Player")
			return true;
	}

	return false;
}

void OverlapObject::UpdateOverlapPosition(physx::PxVec3 pos)
{
	m_currentOverlapPos = pos;
}
