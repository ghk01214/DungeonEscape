#include "pch.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CollisionPairInfo.h"
#include "MessageHandler.h"
#include "Monster.h"

SkillObject::SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, server::OBJECT_TYPE type)
	: GameObject(position, rotation, scale), m_body(nullptr)//, m_objType(type)
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

		if (type == server::OBJECT_TYPE::BOSS)
		{								//스킬이 hit한게 몬스터
			SetRemoveReserved();		//객체 삭제

			game::Message msg{ -1, ProtocolID::WR_REMOVE_ACK };
			msg.objID = m_id;
			msg.objType = m_objType;

			game::MessageHandler::GetInstance()->PushSendMessage(msg);

			auto monster{ dynamic_cast<Monster*>(collider->GetOwnerObject()) };

			if (monster != nullptr)
			{
				game::Message hitMsg{ -1, ProtocolID::WR_HIT_ACK };
				hitMsg.objID = monster->GetID();
				hitMsg.objType = server::OBJECT_TYPE::BOSS;
				hitMsg.hitID = m_id;

				game::MessageHandler::GetInstance()->PushSendMessage(hitMsg);
			}
		}
		else if (type == server::OBJECT_TYPE::PLAYER)
		{
			std::cout << "p\n";
		}
		else
		{
			switch (m_objType)
			{
				case server::OBJECT_TYPE::FIREBALL:
				{
					//std::cout << "파이어볼 맵타격 처리\n";
				}
				break;
				case server::OBJECT_TYPE::ICEBALL:
				{
					//std::cout << "아이스볼 맵타격 처리\n";
				}
				break;
			}

			SetRemoveReserved();		//객체 삭제

			game::Message sendMsg{ -1, ProtocolID::WR_REMOVE_ACK };
			sendMsg.objID = m_id;
			sendMsg.objType = m_objType;

			game::MessageHandler::GetInstance()->PushSendMessage(sendMsg);
		}
	}
}

