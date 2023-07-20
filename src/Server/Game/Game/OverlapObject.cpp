#include "pch.h"
#include "OverlapObject.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "Player.h"
#include "RigidBody.h"
#include "Collider.h"
#include "CustomController.h"
#include "EventHandler.h"

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

	std::vector<Player*> validPlayers = m_monsterAI->SkillRangeCheck_OverlapObject(m_currentScheduleName);

	for (auto& player : validPlayers)
	{
		bool duplicate = IsPlayerDuplicate(player);
		if (duplicate)
			continue;
		else
		{
			ApplyMonsterSkillToPlayer(player);
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

void OverlapObject::ApplyMonsterSkillToPlayer(Player* player)
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
			dragPower = 75.f;
		if (distance < 300)
			dragPower = 30.f;
		if (distance < 200)
			dragPower = 1.f;
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->SetVelocity(PxVec3(0));
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 700.f);
		playerBody->AddForce(ForceMode::Impulse, -xzDir * dragPower);
	}

	else if (m_currentScheduleName == "ATTACK2")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 800.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 200.f);
	}

	else if (m_currentScheduleName == "ATTACK3")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 300.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, -1, 0) * 2000.f);
	}

	else if (m_currentScheduleName == "ATTACK4")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 200.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 200.f);
	}

	else if (m_currentScheduleName == "ROAR")
	{
		player->SetStun(true);
		EventHandler::GetInstance()->AddEvent("PLAYER_STUN_OFF", 4.f, player);			//5초 스턴
	}

	else if (m_currentScheduleName == "RUN")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 700.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 150.f);
		std::cout << "RUN피격확인" << std::endl;
	}
}
