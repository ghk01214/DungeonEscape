#include "pch.h"
#include "TriggerObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "TriggerPairInfo.h"
#include "Player.h"
#include "Monster.h"
#include "CustomController.h"

TriggerObject::TriggerObject(const Vec3& position, const Quat& rotation, const Vec3& scale)
	: GameObject(position, rotation, scale), m_body(nullptr)
{
}

TriggerObject::~TriggerObject()
{
}

void TriggerObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");
	m_body->SetCCDFlag(false);
	m_body->SetKinematic(true);

	SetObjectType(server::OBJECT_TYPE::TRIGGER_OBJECT);
}

void TriggerObject::Update(double timeDelta)
{
	TriggerUpdate();

	GameObject::Update(timeDelta);
}

void TriggerObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();
}

void TriggerObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void TriggerObject::TriggerUpdate()
{
	Trigger_Persistent();
	Trigger_SingleStrike();
}

void TriggerObject::Trigger_Persistent()
{
	if (m_triggerType == server::TRIGGER_TYPE::SINGLE_STRIKE)
		return;

	auto collisionEnter = m_body->GetCollider(0)->GetTriggerInfo(CollisionInfoType::Enter);
	auto collisionExit = m_body->GetCollider(0)->GetTriggerInfo(CollisionInfoType::Exit);

	for (auto& info : collisionEnter)		//enter
	{
		auto collider = info.get()->GetFromCollider();
		auto ownerObj = collider->GetOwnerObject();
		auto player = dynamic_cast<Player*>(ownerObj);
		if (player == nullptr)
			continue;

		player->TriggerZoneStatusChange(m_triggerType, true);
	}

	for (auto& info : collisionExit)		//exit
	{
		std::cout << "TriggerObject::Persistent(collisionExit)\n";
		auto collider = info.get()->GetFromCollider();
		auto ownerObj = collider->GetOwnerObject();
		auto player = dynamic_cast<Player*>(ownerObj);
		if (player == nullptr)
			continue;

		player->TriggerZoneStatusChange(m_triggerType, false);
	}
}

void TriggerObject::Trigger_SingleStrike()
{
	if (m_triggerType != server::TRIGGER_TYPE::SINGLE_STRIKE)
		return;

#pragma region 시간 갱신
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	if (dt > 100.f)
		dt = 0.f;
	m_currentTime += dt;

	if (m_currentTime == 0.f)
	{
		ExcludeTriggerFromSimulation(true);							//0초 : 시뮬레이션 비활성화
		m_originalPosition = m_body->GetPosition();
	}
	else if (m_currentTime > m_startTime && m_currentTime < m_endTime)
	{
		if (m_body->IsExcludedFromSimulation())
		{
			ExcludeTriggerFromSimulation(false);						//시작~끝 : 시뮬레이션 활성화
			std::cout << "활성화\n";
		}
	}
	else if (m_currentTime > m_endTime)
	{
		if (!m_body->IsExcludedFromSimulation())
		{
			ExcludeTriggerFromSimulation(true);						//끝~ : 시뮬레이션 비활성화
			std::cout << "비활성화\n";
		}
		return;
	}
#pragma endregion


#pragma region 충돌체크 + 중복체크
	auto collisionEnter = m_body->GetCollider(0)->GetTriggerInfo(CollisionInfoType::Enter);
	for (auto& info : collisionEnter)				//enter
	{
		auto collider = info.get()->GetFromCollider();
		auto ownerObj = collider->GetOwnerObject();
		auto player = dynamic_cast<Monster*>(ownerObj);
		if (player == nullptr)
			continue;								//조건 1 : 플레이어인가?

		bool duplicate = false;
		ExcludeManagement(collider, duplicate);
		if (duplicate)
			continue;								//조건 2 : 중복은 아닌가?

		player->GetController()->GetBody()->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 500.f);
		// server : 플레이어에게 데미지를 준다는 메시지 전달 필요
	}
#pragma endregion
}

void TriggerObject::ExcludeTriggerFromSimulation(bool value)
{
	if (value)
	{
		m_originalPosition = m_body->GetPosition();
		m_body->SetPosition(Vec3(-100000.f, -1000.f, -1000.f), true);
		m_body->ExcludeFromSimulation(true);
	}
	else
	{
		m_body->SetPosition(FROM_PX3(m_originalPosition), true);
		m_body->ExcludeFromSimulation(false);
	}
}

void TriggerObject::ExcludeManagement(Collider* collider, bool& duplicate)
{
	for (auto& col : m_excludeColliders)
	{
		if (col == collider)		// exclude목록에 이미 있는 콜라이더
		{
			duplicate = true;
			return;
		}
	}

	duplicate = false;				// exclude목록에 없던 콜라이더. 컨테이너에 추가
	m_excludeColliders.emplace_back(collider);
}

void TriggerObject::RestoreOneTimeEffect()
{
	m_excludeColliders.clear();						//중복목록 초기화
	ExcludeTriggerFromSimulation(false);			//시뮬레이션 참가
	m_currentTime = 0.f;							//활성화 시간 복구
}

void TriggerObject::SetTriggerType(server::TRIGGER_TYPE type, float startTime, float endTime)
{
	m_triggerType = type;
	m_startTime = startTime;
	m_endTime = endTime;
	m_currentTime = 0.f;

	if (type == server::TRIGGER_TYPE::SINGLE_STRIKE)
		ExcludeTriggerFromSimulation(true);
}

server::TRIGGER_TYPE TriggerObject::GetTriggerType()
{
	return m_triggerType;
}
