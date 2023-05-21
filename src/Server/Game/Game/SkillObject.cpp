#include "pch.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CollisionPairInfo.h"


SkillObject::SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, server::OBJECT_TYPE type)
	: GameObject(position, rotation, scale), m_body(nullptr), m_objType(type)
{
}

SkillObject::~SkillObject()
{
}

void SkillObject::Init()
{
	m_body = AddComponent<RigidBody>(L"RigidBody");
	m_body->SetCCDFlag(true);
	m_body->SetKinematic(false);
}

void SkillObject::Update(double timeDelta)
{
	ActionsWhenHit();


	GameObject::Update(timeDelta);
}

void SkillObject::LateUpdate(double timeDelta)
{
	m_body->ClearCollidersCollisionInfo();

	m_transform->ConvertPX(m_body->GetGlobalPose());
}

void SkillObject::Release()
{
	m_body = nullptr;
	GameObject::Release();
}

void SkillObject::ActionsWhenHit()
{
	auto enterInfos = m_body->GetCollider(0)->GetCollisionInfo(CollisionInfoType::Enter);
	for (auto& info : enterInfos)
	{
		auto collider = info.get()->GetFromCollider();
		server::OBJECT_TYPE type = collider->GetOwnerObject()->GetObjectType();
		if (type == server::OBJECT_TYPE::MONSTER)		
		{								//스킬이 hit한게 몬스터
			SetRemoveReserved();		//객체 삭제

			switch (m_objType)
			{
				case server::OBJECT_TYPE::FIREBALL:
				std::cout << "파이어볼 몬스터타격 처리" << std::endl;
				//msg써서 파이어볼처리
				break;
				case server::OBJECT_TYPE::ICEBALL:
				std::cout << "아이스볼 몬스터타격 처리" << std::endl;
				//msg써서 아이스볼처리
				break;
			}
		}
		else if (type == server::OBJECT_TYPE::PLAYER)
		{	
			std::cout << "p" << std::endl;
		}
		else
		{
			std::cout << "파이어볼 맵타격 처리" << std::endl;
			SetRemoveReserved();		//객체 삭제
		}
	}
}

