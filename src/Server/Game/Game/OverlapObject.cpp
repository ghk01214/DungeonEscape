#include "pch.h"
#include "OverlapObject.h"
#include "MonsterAI.h"
#include "Player.h"
#include "RigidBody.h"
#include "CustomController.h"

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

	//for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	//{
	//	const PxOverlapHit& hit = overlapBuffer.getTouch(i);
	//	PxRigidActor* actor = hit.actor;
	//	const char* ptr = actor->getName();

	//	auto body = static_cast<RigidBody*>(actor->userData);
	//
	//	std::string name = body->GetName();
	//	if (name == "Player")
	//	{
	//		auto player = dynamic_cast<Player*>(body->GetOwnerObject());
	//		bool duplicate = IsPlayerDuplicate(player);
	//		if (duplicate)
	//		{
	//			continue;
	//		}
	//		else
	//		{
	//			ApplyMonsterSkillToPlayer(player);
	//			m_duplicates.emplace_back(player);
	//		}
	//	}
	//}

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

	//AddSkillSize로 ai에 추가된 MonsterSkill의 이름을 기준으로 작성
	if (m_currentScheduleName == "ATTACK1")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->SetVelocity(PxVec3(0));
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 700.f);
		playerBody->AddForce(ForceMode::Impulse, -xzDir * 200.f);
	}
	else if (m_currentScheduleName == "ATTACK2")
	{
		playerBody->SetVelocity(PxVec3(0));
		playerController->BounceFromAttack();
		playerBody->AddForce(ForceMode::Impulse, xzDir * 800.f);
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 200.f);
	}

}
