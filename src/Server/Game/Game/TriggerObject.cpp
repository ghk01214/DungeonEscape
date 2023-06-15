#include "pch.h"
#include "TriggerObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "TriggerPairInfo.h"
#include "Player.h"

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

	if (m_invalid)
		m_body->ExcludeFromSimulation(true);		//1회성 트리거일 경우 삭제
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
		std::cout << "TriggerObject::Persistent(collisionEnter)" << std::endl;
		auto collider = info.get()->GetFromCollider();
		auto ownerObj = collider->GetOwnerObject();
		auto player = dynamic_cast<Player*>(ownerObj);
		if (player == nullptr)
			continue;

		player->TriggerZoneStatusChange(m_triggerType, true);
	}

	for (auto& info : collisionExit)		//exit
	{
		std::cout << "TriggerObject::Persistent(collisionExit)" << std::endl;
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

	auto collisionEnter = m_body->GetCollider(0)->GetTriggerInfo(CollisionInfoType::Enter);
	for (auto& info : collisionEnter)		//enter
	{
		auto collider = info.get()->GetFromCollider();
		auto ownerObj = collider->GetOwnerObject();
		auto player = dynamic_cast<Player*>(ownerObj);
		if (player == nullptr)
			continue;

		// player->givedmg() 이런방식으로 사용. 클라이언트로 플레이어가 데미지를 입었음을 보내야함.
		m_invalid = true;		//타격을 입혔으니, 이 타격 트리거는 다음 업데이트 루프부터 무효.
	}

}

void TriggerObject::SetTriggerType(server::TRIGGER_TYPE type)
{
	m_triggerType = type;
}

server::TRIGGER_TYPE TriggerObject::GetTriggerType()
{
	return m_triggerType;
}
