﻿#include "pch.h"
#include "SkillObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CollisionPairInfo.h"
#include "MessageHandler.h"
#include "EventHandler.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "PhysDevice.h"

#include "Player.h"
#include "Monsters.hpp"

SkillObject::SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, SKILLOBJECTTYPE skilltype, GameObject* target, GameObject* owner)
	: GameObject(position, rotation, scale), m_body(nullptr), m_skillType(skilltype), m_target(target), m_owner(owner)
{
}

SkillObject::~SkillObject()
{
}

void SkillObject::Init()
{
	m_id = game::MessageHandler::GetInstance()->NewObjectID();

	m_body = AddComponent<RigidBody>(L"RigidBody");
	m_body->SetCCDFlag(true);
	m_body->SetKinematic(false);

	switch (m_skillType)
	{
		case SKILLOBJECTTYPE::PLAYER_FIREBALL:
		{
			m_name = L"PLAYER FIREBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_SPHERE;
			m_objType = server::OBJECT_TYPE::PLAYER_FIREBALL;

			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_ICEBALL:
		{
			m_name = L"PLAYER ICEBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_SPHERE;
			m_objType = server::OBJECT_TYPE::PLAYER_ICEBALL;

			m_body->SetMass(1.f);
			m_body->AddCollider<BoxCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_THUNDERBALL:
		{
			m_name = L"PLAYER THUNDERBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_SPHERE;
			m_objType = server::OBJECT_TYPE::PLAYER_THUNDERBALL;

			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 500.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_POISONBALL:
		{
			m_name = L"PLAYER POISONBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_SPHERE;
			m_objType = server::OBJECT_TYPE::PLAYER_POISONBALL;

			m_body->SetMass(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST1_BALL:
		{
			m_name = L"WEEPER_CAST1_BALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_FIREBALL;

			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			auto collider = m_body->GetCollider(0);
			collider->SetRestitutionCombineMode(PhysicsCombineMode::Max);
			collider->SetRestitution(0.8f);

			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
			EventHandler::GetInstance()->AddEvent("SKILL_GUIDESTART", 3.5f, this);		//3.2초후 추적시작
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL:
		{
			m_name = L"WEEPER_CAST2_BALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_FIREBALL;

			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());

			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			m_name = L"WEEPER_CAST2_BALL_SCATTER";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_FIREBALL;

			m_transform->SetScale(100.f, 100.f, 100.f);

			m_body->SetMass(10.f);
			m_body->AddRandomTorque_Vertical(ForceMode::Impulse, 200.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());


			SetAttribute(SkillObject::SKILLATTRIBUTE::DESCENDING, true);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST3_BALL:
		{
			m_name = L"WEEPER_CAST3_BALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_FIREBALL;

			m_transform->SetScale(100.f, 100.f, 100.f);

			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());

			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST4_BALL:
		{
			m_name = L"WEEPER_CAST4_BALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_FIREBALL;

			m_transform->SetScale(100.f, 100.f, 100.f);

			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());

			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
		}
		break;

		case SKILLOBJECTTYPE::MONSTER_ICEBALL:
		{
			m_name = L"MONSTER ICEBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_ICEBALL;

			m_body->SetMass(1.f);
			m_body->AddCollider<BoxCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::MONSTER_THUNDERBALL:
		{
			m_name = L"MONSTER THUNDERBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_THUNDERBALL;

			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::MONSTER_POISONBALL:
		{
			m_name = L"MONSTER POISONBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_SPHERE;
			m_objType = server::OBJECT_TYPE::MONSTER_POISONBALL;

			m_body->SetMass(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;
		default:
		break;
	}

	if (m_owner->GetObjectType() == server::OBJECT_TYPE::PLAYER)
	{
		m_flagPlayer = true;
		m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::SKILLOBJECT_PLAYER,
				static_cast<PhysicsLayers>(static_cast<int>(PhysicsLayers::PLAYER) | static_cast<int>(PhysicsLayers::SKILLOBJECT_PLAYER)));
	}
	else
	{
		m_flagPlayer = false;
		m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::SKILLOBJECT_MONSTER,
				static_cast<PhysicsLayers>(static_cast<int>(PhysicsLayers::MONSTER) | static_cast<int>(PhysicsLayers::SKILLOBJECT_MONSTER)));
	}

	//ServerMessage_SkillInit();
	game::Message sendMsg{ -1, ProtocolID::WR_ADD_OBJ_ACK };
	sendMsg.objType = m_objType;
	sendMsg.objID = m_id;

	game::MessageHandler::GetInstance()->PushSendMessage(sendMsg);
}

void SkillObject::Update(double timeDelta)
{
	//이동로직 (플레이어는 SkillPattern()에서 최초 힘 적용하면 끝. 보스 스킬만 추가적인 이동로직이 있다)
	Handle_Attribute();


	//충돌로직
	if (IsPlayerSkill())
		HandlePlayerSkillCollision();
	else
		HandleMonsterSkillCollision();

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

void SkillObject::ServerMessage_SkillInit()
{
	game::Message sendMsg{ -1, ProtocolID::WR_ADD_OBJ_ACK };
	sendMsg.objType = m_objType;
	sendMsg.objID = m_id;

	game::MessageHandler::GetInstance()->PushSendMessage(sendMsg);
}

void SkillObject::ServerMessage_SkillHit()
{
	//스킬 오브젝트 객체 삭제 전달.
	//추후 피격 잔상 위치 같이 추가적으로 더 정보를 전달할 수 있음.

	game::Message sendMsg{ -1, ProtocolID::WR_REMOVE_ACK };
	sendMsg.objID = m_id;
	sendMsg.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(sendMsg);
}

bool SkillObject::IsPlayerSkill()
{
	if (m_flagPlayer)
		return true;

	else
		return false;
}

void SkillObject::HandlePlayerSkillCollision()
{
	auto enterInfos = m_body->GetCollider(0)->GetCollisionInfo(CollisionInfoType::Enter);
	for (auto& info : enterInfos)
	{
		auto collider = info.get()->GetFromCollider();
		server::OBJECT_TYPE type = collider->GetOwnerObject()->GetObjectType();

		//플레이어스킬 : 보스타격
		if (type == server::OBJECT_TYPE::BOSS)		//피격대상 : 보스
		{
			auto monster{ dynamic_cast<Monster*>(collider->GetOwnerObject()) };

			if (monster != nullptr and monster->IsDead() == false)
			{
				//Switch : 몬스터 종류
				switch (monster->GetFBXType())
				{
					case server::FBX_TYPE::WEEPER1:
					case server::FBX_TYPE::WEEPER2:
					case server::FBX_TYPE::WEEPER3:
					case server::FBX_TYPE::WEEPER4:
					case server::FBX_TYPE::WEEPER5:
					case server::FBX_TYPE::WEEPER6:
					case server::FBX_TYPE::WEEPER7:
					case server::FBX_TYPE::WEEPER_EMISSIVE:
					{
						auto weeper{ dynamic_cast<Weeper*>(monster) };

						if (weeper != nullptr)
						{
							//추후 추가
						}
					}
					break;
					case server::FBX_TYPE::BLUE_GOLEM:
					case server::FBX_TYPE::GREEN_GOLEM:
					case server::FBX_TYPE::RED_GOLEM:
					{
						auto golem{ dynamic_cast<Golem*>(monster) };

						if (golem != nullptr)
						{
							//추후 추가
						}
					}
					break;
					case server::FBX_TYPE::DRAGON:
					{
						auto dragon{ dynamic_cast<Dragon*>(monster) };

						if (dragon != nullptr)
						{
							//추후 추가
						}
					}
					break;
					default:
					break;
				}

				SetRemoveReserved();						//객체 삭제
				m_body->ExcludeFromSimulation(true);
				ServerMessage_SkillHit();					//서버 메시지 처리
			}
		}

		//플레이어스킬 : 팀킬
		else if (type == server::OBJECT_TYPE::PLAYER)
		{
			SetRemoveReserved();						//객체 삭제
			ServerMessage_SkillHit();					//서버 메시지 처리
		}

		//플레이어스킬 : 맵 타격
		else if (type == server::OBJECT_TYPE::MAP_OBJECT)
		{
			switch (m_skillType)
			{
				case SKILLOBJECTTYPE::PLAYER_FIREBALL:
				{
					//std::cout << "파이어볼 맵타격 처리\n";
				}
				break;
				case SKILLOBJECTTYPE::PLAYER_ICEBALL:
				{
					//std::cout << "아이스볼 맵타격 처리\n";
				}
				break;
				case SKILLOBJECTTYPE::PLAYER_THUNDERBALL:
				{
					//std::cout << "파이어볼 맵타격 처리\n";
				}
				break;
				case SKILLOBJECTTYPE::PLAYER_POISONBALL:
				{
					//std::cout << "아이스볼 맵타격 처리\n";
				}
				break;
			}

			SetRemoveReserved();						//객체 삭제
			ServerMessage_SkillHit();					//서버 메시지 처리
		}
	}
}

void SkillObject::HandleMonsterSkillCollision()
{
	auto enterInfos = m_body->GetCollider(0)->GetCollisionInfo(CollisionInfoType::Enter);
	for (auto& info : enterInfos)
	{
		auto collider = info.get()->GetFromCollider();
		server::OBJECT_TYPE type = collider->GetOwnerObject()->GetObjectType();

		//보스스킬 : 플레이어 타격
		if (type == server::OBJECT_TYPE::PLAYER)
		{
			auto player{ dynamic_cast<Player*>(collider->GetOwnerObject()) };

			if (player != nullptr and player->IsDead() == false)
			{
				switch (player->GetFBXType())
				{
					case server::FBX_TYPE::NANA:
					{
						//플레이어 나나 피격
					}
					break;
					case server::FBX_TYPE::MISTIC:
					{
						//플레이어 미스틱 피격
					}
					break;
					case server::FBX_TYPE::CARMEL:
					{
						//플레이어 카멜 피격
					}
					break;
				}

				SetRemoveReserved();						//객체 삭제
				ServerMessage_SkillHit();					//서버 메시지 처리
			}
		}

		//보스스킬 : 맵 타격
		else if (type == server::OBJECT_TYPE::MAP_OBJECT)
		{
			switch (m_skillType)
			{
				case SKILLOBJECTTYPE::PLAYER_FIREBALL:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillHit();					//서버 메시지 처리
				}
				break;
				case SKILLOBJECTTYPE::PLAYER_POISONBALL:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillHit();					//서버 메시지 처리
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST1_BALL:
				{
					if (m_mapEncountered)
					{
						SetRemoveReserved();						//객체 삭제
						ServerMessage_SkillHit();					//서버 메시지 처리
					}
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillHit();					//서버 메시지 처리
				}
				break;
			}


		}
	}

	auto exitInfos = m_body->GetCollider(0)->GetCollisionInfo(CollisionInfoType::Exit);
	for (auto& info : exitInfos)
	{
		auto collider = info.get()->GetFromCollider();
		server::OBJECT_TYPE type = collider->GetOwnerObject()->GetObjectType();

		if (m_skillType == SkillObject::SKILLOBJECTTYPE::WEEPER_CAST1_BALL)
		{
			SetAttribute(SkillObject::SKILLATTRIBUTE::GUIDED, false);
			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, false);
			m_body->SetMass(10.f);
			m_mapEncountered = true;
		}
	}
}

void SkillObject::PlayerSkillFire(physx::PxVec3 dir)
{
	if (m_body)
	{
		m_body->AddForce(ForceMode::Impulse, dir * m_firePower);
	}
}

void SkillObject::MonsterSkillFire(physx::PxVec3 dir)
{
	if (m_body)
	{
		m_body->AddForce(ForceMode::Impulse, dir * m_firePower);
	}
}

void SkillObject::Handle_Attribute()
{
	Attirbute_Levitate();
	Attribute_Guide();
	Attribute_Ascending();
}

void SkillObject::Attirbute_Levitate()
{
	if (m_skillAttrib & SKILLATTRIBUTE::LEVITATE)
	{
		physx::PxVec3 gravity = PhysDevice::GetInstance()->GetGravity();
		m_body->AddForce(ForceMode::Force, -gravity * m_body->GetMass());
	}
}

void SkillObject::Attribute_Guide()
{
	if (!m_target)								//타겟없으면 취소
		return;

	if (m_body->IsExcludedFromSimulation())		//삭제예정이면 취소
		return;

	if (m_skillAttrib & SKILLATTRIBUTE::GUIDED)
	{
		float guidePower = 15.f;

		physx::PxVec3 TargetPos = TO_PX3(m_target->GetTransform()->GetPosition());
		physx::PxVec3 TargetDir = TargetPos - m_body->GetPosition();
		TargetDir.normalize();

		m_body->AddForce(ForceMode::Force, TargetDir * guidePower);
	}
}

void SkillObject::Attribute_Ascending()
{
	if (m_body->IsExcludedFromSimulation())		//삭제예정이면 취소
		return;

	if (m_skillAttrib & SKILLATTRIBUTE::ASCENDING)
	{
		physx::PxVec3 up(0, 1, 0);
		float power = 50.f;

		m_body->SetVelocity(up * power);
	}
}

void SkillObject::Attribute_Descending()
{
	if (m_body->IsExcludedFromSimulation())		//삭제예정이면 취소
		return;

	if (m_skillAttrib & SKILLATTRIBUTE::ASCENDING)
	{
		physx::PxVec3 up(0, -1, 0);
		float power = 5.f;

		m_body->SetVelocity(up * power);
	}
}

void SkillObject::SetAttribute(SKILLATTRIBUTE attrib, bool set)
{
	if (set)
	{
		m_skillAttrib = static_cast<SKILLATTRIBUTE>(m_skillAttrib | attrib);
	}
	else
	{
		m_skillAttrib = static_cast<SKILLATTRIBUTE>(m_skillAttrib & ~attrib);
	}
}

SkillObject::SKILLATTRIBUTE SkillObject::GetAttribute()
{
	return m_skillAttrib;
}
