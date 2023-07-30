#include "pch.h"
#include "OverlapObject2.h"
#include "Player.h"
#include "Monster.h"
#include "PillarObject.h"
#include "RigidBody.h"
#include "Collider.h"
#include "CustomController.h"
#include "EventHandler.h"
#include "RaycastHit.h"
#include "PhysDevice.h"
#include "physx_utils.h"

#include "MessageHandler.h"

using namespace physx;
using namespace std;

OverlapObject2::OverlapObject2(Player* player):
	m_player(player)
{
}

OverlapObject2::~OverlapObject2()
{
}

void OverlapObject2::Init()
{
	m_box = new PxBoxGeometry(physx::PxVec3(150, 200, 200));
	if (!m_box->isValid())
		return;
}

void OverlapObject2::Update()
{
	Active();
}

void OverlapObject2::Release()
{
	Deactivate();
	m_player = nullptr;
	SafeDelete(m_box);
}

void OverlapObject2::UpdateOverlapPosition(physx::PxVec3 pos, physx::PxQuat quat)
{
	m_pos = pos;
	m_rot = quat;
}

void OverlapObject2::Activate()
{
	m_active = true;
}

void OverlapObject2::Deactivate()
{
	m_duplicates.clear();
	m_active = false;
}

void OverlapObject2::Active()
{
	if (!m_active)
		return;

#pragma region 몬스터 처리
	std::vector<Monster*> validMonsters = OverlapCheck_Monster();		//overlap진행

	for (auto& monster : validMonsters)
	{
		bool duplicate = IsObjectDuplicate(monster);
		if (duplicate)
			continue;
		else
		{
			//bool applied = ApplySkillToMonster(monster);
			//if (applied)
			//	m_duplicates.emplace_back(monster);
			ApplySkillToMonster(monster);
			m_duplicates.emplace_back(monster);

		}
	}
#pragma endregion

#pragma region 기둥 처리
	std::vector<PillarObject*> validPillars = OverlapCheck_Pillar();		//overlap진행

	for (auto& pillar : validPillars)
	{
		bool duplicate = IsObjectDuplicate(pillar);
		if (duplicate)
			continue;
		else
		{
			pillar->ReceivedAttack_SingleAttack();
			m_duplicates.emplace_back(pillar);
		}
	}
#pragma endregion
}

std::vector<Monster*> OverlapObject2::OverlapCheck_Monster()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::vector<Monster*> validPtrs;

	if (!m_box)
		return validPtrs;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

	physx::PxTransform trans;								//스킬 위치, 회전값 부여
	trans.p = m_pos;
	trans.q = m_rot;

	PxQueryFilterData filterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);		//overlap진행
	constexpr PxU32 bufferSize = 10;  // Set this to whatever size you need
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer overlapBuffer(hitBuffer, bufferSize);

	query->Overlap(*m_box, trans, filterData, &overlapBuffer);

	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxShape* shape = hit.shape;
		PxRigidActor* actor = shape->getActor();

		auto body = static_cast<RigidBody*>(actor->userData);

		GameObject* obj = body->GetOwnerObject();
		if (!obj)
			continue;											// 1 : ownerobj 유효포인터 체크
		std::wstring name = obj->GetName();
		if (name == L"Weeper" || name == L"Golem")				// 2 : 이름체크
		{
			auto monster = dynamic_cast<Monster*>(obj);			// 3 : 리턴 컨테이너에 등록
			validPtrs.emplace_back(monster);
			int size = validPtrs.size();
		}
	}

	return validPtrs;
}

std::vector<PillarObject*> OverlapObject2::OverlapCheck_Pillar()
{
	auto physDevice = PhysDevice::GetInstance();
	auto query = physDevice->GetQuery();

	std::vector<PillarObject*> validPtrs;

	if (!m_box)
		return validPtrs;									//여기 걸리면 SkillAdd()의 오류. 각 Monster의 Init수정 요구

	physx::PxTransform trans;								//스킬 위치, 회전값 부여
	trans.p = m_pos;
	trans.q = m_rot;

	PxQueryFilterData filterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC);		//overlap진행
	constexpr PxU32 bufferSize = 10;  // Set this to whatever size you need
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer overlapBuffer(hitBuffer, bufferSize);

	query->Overlap(*m_box, trans, filterData, &overlapBuffer);

	for (PxU32 i = 0; i < overlapBuffer.getNbTouches(); ++i)
	{
		const PxOverlapHit& hit = overlapBuffer.getTouch(i);
		PxShape* shape = hit.shape;
		PxRigidActor* actor = shape->getActor();

		auto body = static_cast<RigidBody*>(actor->userData);

		GameObject* obj = body->GetOwnerObject();
		if (!obj)
			continue;											// 1 : ownerobj 유효포인터 체크
		std::wstring name = obj->GetName();
		if (name == L"PILLAR BRIDGE")				// 2 : 이름체크
		{
			auto pillar = dynamic_cast<PillarObject*>(obj);			// 3 : 리턴 컨테이너에 등록
			validPtrs.emplace_back(pillar);
			int size = validPtrs.size();
		}
	}

	return validPtrs;
}

bool OverlapObject2::IsObjectDuplicate(GameObject* obj)
{
	if (std::find(m_duplicates.begin(), m_duplicates.end(), obj) == m_duplicates.end())
		return false;

	return true;
}

bool OverlapObject2::ApplySkillToMonster(Monster* monster)
{
	std::wstring monstername = monster->GetName();
	if (monstername == L"Weeper")
	{
	}
	else
	{
	}


	monster->GetDamaged(10);
	cout << "몬스터에게 데미지를 줬다." << endl;
	return true;
}


void OverlapObject2::ServerMessage_RenderEffect(Player* player, server::EFFECT_TYPE type)
{
	//auto effectPos{ player->LocationForBilboard(m_monsterAI) };

	game::TIMER_EVENT ev{ ProtocolID::WR_RENDER_EFFECT_ACK };
	ev.objID = player->GetID();
	ev.state = magic_enum::enum_integer(type);
	//ev.effectPos = effectPos;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);
}

