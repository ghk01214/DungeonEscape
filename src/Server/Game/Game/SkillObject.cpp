#include "pch.h"
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

#include "ObjectManager.h"
#include "Layer.h"
#include "Player.h"
#include "Monsters.hpp"
#include "MonsterAI.h"
#include "WeeperAI.h"
#include "PillarObject.h"
#include "CustomController.h"


SkillObject::SkillObject(const Vec3& position, const Quat& rotation, const Vec3& scale, SKILLOBJECTTYPE skilltype, GameObject* target, GameObject* owner)
	: GameObject(position, rotation, scale), m_body(nullptr), m_skillType(skilltype), m_target(target), m_owner(owner)
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

	switch (m_skillType)
	{
		case SKILLOBJECTTYPE::PLAYER_FIREBALL:
		{
			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_ICEBALL:
		{
			m_body->SetMass(1.f);
			m_body->AddCollider<BoxCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_THUNDERBALL:
		{
			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 500.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_POISONBALL:
		{
			m_body->SetMass(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::PLAYER_METEOR:
		{
			m_body->SetMass(100.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 0.f;
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST1_BALL:
		{
			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			auto collider = m_body->GetCollider(0);
			collider->SetRestitutionCombineMode(PhysicsCombineMode::Max);
			collider->SetRestitution(0.8f);

			SetAttribute(SkillObject::SKILLATTRIBUTE::NEW_LEVITATE, true);
			EventHandler::GetInstance()->AddEvent("SKILL_GUIDESTART", 3.6f, this);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL:
		{
			m_body->SetMass(3.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 400.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());

			SetAttribute(SkillObject::SKILLATTRIBUTE::NEW_LEVITATE, true);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			m_transform->SetScale(100.f, 100.f, 100.f);

			m_body->SetMass(20.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());


			SetAttribute(SkillObject::SKILLATTRIBUTE::DESCENDING, true);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST3_BALL:
		{
			m_body->SetMass(10.f);
			m_body->AddRandomTorque(ForceMode::Impulse, 100.f);
			m_body->GetBody()->setAngularDamping(0.5f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			auto collider = m_body->GetCollider(0);
			collider->SetRestitutionCombineMode(PhysicsCombineMode::Max);
			collider->SetRestitution(0.8f);

			SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
			EventHandler::GetInstance()->AddEvent("SKILL_LEVITATE_END", 0.5f, this);
		}
		break;

		case SKILLOBJECTTYPE::WEEPER_CAST4_BALL:
		{
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
			m_body->SetMass(1.f);
			m_body->AddCollider<BoxCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::MONSTER_THUNDERBALL:
		{
			m_body->SetMass(1.f);
			m_body->AddCollider<SphereCollider>(GetTransform()->GetScale());
			m_firePower = 200.f;
		}
		break;

		case SKILLOBJECTTYPE::MONSTER_POISONBALL:
		{
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
				static_cast<PhysicsLayers>(PLAYER | SKILLOBJECT_PLAYER));
	}
	else
	{
		m_flagPlayer = false;
		m_body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::SKILLOBJECT_MONSTER,
				static_cast<PhysicsLayers>(static_cast<int>(PhysicsLayers::MONSTER) | static_cast<int>(PhysicsLayers::SKILLOBJECT_MONSTER)));
	}

	ServerMessage_SkillInit();
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
	m_id = game::MessageHandler::GetInstance()->NewObjectID();

	switch (m_skillType)
	{
		case SKILLOBJECTTYPE::PLAYER_FIREBALL:
		{
			m_name = L"PLAYER FIREBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_FIREBALL;
			m_objType = server::OBJECT_TYPE::PLAYER_FIREBALL;
		}
		break;
		case SKILLOBJECTTYPE::PLAYER_ICEBALL:
		{
			m_name = L"PLAYER ICEBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_ICEBALL;
			m_objType = server::OBJECT_TYPE::PLAYER_ICEBALL;
		}
		break;
		case SKILLOBJECTTYPE::PLAYER_THUNDERBALL:
		{
			m_name = L"PLAYER THUNDERBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_THUNDERBALL;
			m_objType = server::OBJECT_TYPE::PLAYER_THUNDERBALL;
		}
		break;
		case SKILLOBJECTTYPE::PLAYER_POISONBALL:
		{
			m_name = L"PLAYER POISONBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_POISONBALL;
			m_objType = server::OBJECT_TYPE::PLAYER_POISONBALL;
		}
		break;
		case SKILLOBJECTTYPE::PLAYER_METEOR:
		{
			m_name = L"PLAYER FIREBALL";
			m_fbxType = server::FBX_TYPE::PLAYER_METEOR;
			m_objType = server::OBJECT_TYPE::PLAYER_METEOR;			//어울리는거로 수정
		}
		break;
		case SKILLOBJECTTYPE::WEEPER_CAST1_BALL:
		{
			m_name = L"WEEPER_CAST1_BALL";
			m_fbxType = server::FBX_TYPE::WEEPER_CAST1_BALL;
			m_objType = server::OBJECT_TYPE::WEEPER_CAST1_BALL;
		}
		break;
		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL:
		{
			m_name = L"WEEPER_CAST2_BALL";
			m_fbxType = server::FBX_TYPE::WEEPER_CAST2_BALL;
			m_objType = server::OBJECT_TYPE::WEEPER_CAST2_BALL;
		}
		break;
		case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER:
		{
			m_name = L"WEEPER_CAST2_BALL_SCATTER";
			m_fbxType = server::FBX_TYPE::WEEPER_CAST2_BALL_SCATTER;
			m_objType = server::OBJECT_TYPE::WEEPER_CAST2_BALL_SCATTER;
		}
		break;
		case SKILLOBJECTTYPE::WEEPER_CAST3_BALL:
		{
			m_name = L"WEEPER_CAST3_BALL";
			m_fbxType = server::FBX_TYPE::WEEPER_CAST3_BALL;
			m_objType = server::OBJECT_TYPE::WEEPER_CAST3_BALL;
		}
		break;
		case SKILLOBJECTTYPE::WEEPER_CAST4_BALL:
		{
			m_name = L"WEEPER_CAST4_BALL";
			m_fbxType = server::FBX_TYPE::WEEPER_CAST4_BALL;
			m_objType = server::OBJECT_TYPE::WEEPER_CAST4_BALL;
		}
		break;
		case SKILLOBJECTTYPE::MONSTER_ICEBALL:
		{
			m_name = L"MONSTER ICEBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_ICEBALL;
			m_objType = server::OBJECT_TYPE::MONSTER_ICEBALL;
		}
		break;
		case SKILLOBJECTTYPE::MONSTER_THUNDERBALL:
		{
			m_name = L"MONSTER THUNDERBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_THUNDERBALL;
			m_objType = server::OBJECT_TYPE::MONSTER_THUNDERBALL;
		}
		break;
		case SKILLOBJECTTYPE::MONSTER_POISONBALL:
		{
			m_name = L"MONSTER POISONBALL";
			m_fbxType = server::FBX_TYPE::MONSTER_POISONBALL;
			m_objType = server::OBJECT_TYPE::MONSTER_POISONBALL;
		}
		break;
		default:
		break;
	}

	//std::cout << "skill add : " << m_id << ", " << magic_enum::enum_name(m_objType) << "\n";
	for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
		ev.objType = m_objType;
		ev.objID = m_id;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}

	m_sendRemove = 0;
	m_sendAdd = SEND_AGAIN;
}

void SkillObject::ServerMessage_SkillHit()
{
	for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TIMER_EVENT ev{ ProtocolID::WR_SKILL_HIT_ACK };
		ev.objID = m_id;
		ev.objType = m_objType;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}
}

void SkillObject::ServerMessage_SkillRemove()
{
	//스킬 오브젝트 객체 삭제 전달.
	//추후 피격 잔상 위치 같이 추가적으로 더 정보를 전달할 수 있음.
	//std::cout << "skill remove : " << m_id << ", " << magic_enum::enum_name(m_objType) << "\n";
	for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
		ev.objID = m_id;
		ev.objType = m_objType;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}

	m_sendRemove = SEND_AGAIN;
}

bool SkillObject::IsPlayerSkill()
{
	if (m_flagPlayer)
		return true;

	else
		return false;
}

void SkillObject::SendAddAgain()
{
	if (m_sendAdd <= 0)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_ADD_OBJ_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	--m_sendAdd;
}

void SkillObject::SendRemoveAgain()
{
	if (m_sendRemove <= 0)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_REMOVE_ACK };
	ev.objID = m_id;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	--m_sendRemove;
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

						if (weeper)
						{
							weeper->GetDamaged(10);
							SetRemoveReserved();						//객체 삭제
							ServerMessage_SkillRemove();					//서버 메시지 처리
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
							golem->GetDamaged(10);
							SetRemoveReserved();						//객체 삭제
							ServerMessage_SkillRemove();					//서버 메시지 처리
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

				m_body->ExcludeFromSimulation(true);
				SetRemoveReserved();						//객체 삭제
				ServerMessage_SkillRemove();					//서버 메시지 처리
			}


		}

		//플레이어스킬 : 팀킬
		else if (type == server::OBJECT_TYPE::PLAYER)
		{
			SetRemoveReserved();						//객체 삭제
			ServerMessage_SkillRemove();					//서버 메시지 처리
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
			ServerMessage_SkillRemove();					//서버 메시지 처리
		}

		//플레이어스킬 : 돌기둥 다리 타격
		else if (type == server::OBJECT_TYPE::PHYSX_OBJECT)
		{
			switch (m_skillType)
			{
				case SKILLOBJECTTYPE::PLAYER_METEOR:
				{
					auto pillar = dynamic_cast<PillarObject*>(collider->GetOwnerObject());
					if (pillar)
					{
						pillar->ReceivedAttack_Meteor();
						SetAttribute(SkillObject::SKILLATTRIBUTE::GUIDED_METEOR, false);
						SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, false);
					}
				}
				break;
			}

			//SetRemoveReserved();						//객체 삭제
			//ServerMessage_SkillHit();					//서버 메시지 처리
			//삭제는 5초 후 진행		GimmikMeteor
			EventHandler::GetInstance()->AddEvent("REMOVE_PLAYER_METEOR", 3.f, this);
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

				if (m_skillType == SKILLOBJECTTYPE::WEEPER_CAST2_BALL_NUCLEAR)
				{
					Nuclear_Attribute_Explosion();
				}

				SetRemoveReserved();						//객체 삭제
				ServerMessage_SkillRemove();					//서버 메시지 처리


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
					ServerMessage_SkillRemove();					//서버 메시지 처리
				}
				break;
				case SKILLOBJECTTYPE::PLAYER_POISONBALL:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillRemove();					//서버 메시지 처리
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST1_BALL:
				{
					if (m_mapEncountered)
					{
						SetRemoveReserved();						//객체 삭제
						ServerMessage_SkillRemove();					//서버 메시지 처리
					}
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_SCATTER:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillRemove();					//서버 메시지 처리
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST2_BALL_NUCLEAR:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillRemove();					//서버 메시지 처리

					if (m_skillAttrib & SKILLATTRIBUTE::NUCLEAR)
					{
						Nuclear_Attribute_Explosion();
					}
				}
				break;
				case SKILLOBJECTTYPE::WEEPER_CAST3_BALL:
				{
					SetRemoveReserved();						//객체 삭제
					ServerMessage_SkillRemove();					//서버 메시지 처리
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

void SkillObject::WeeperNuclearFire()
{
	auto weeper = dynamic_cast<Weeper*>(m_owner);
	auto targetPlayer = weeper->GetAI()->GetTarget();
	physx::PxVec3 dir = TO_PX3(targetPlayer->GetControllerPosition() - weeper->GetControllerPosition());
	dir.normalize();
	float power = 3500.f;

	m_skillType = SkillObject::SKILLOBJECTTYPE::WEEPER_CAST2_BALL_NUCLEAR;							//충돌판단을 위한 타입변경
	SetAttribute(SKILLATTRIBUTE::NEW_LEVITATE, false);													//공중 attirb 제거
	m_body->SetMass(100.f);																			//무겁게 변경
	m_body->AddForce(ForceMode::Acceleration, dir * power);												//던진다
}

void SkillObject::Handle_Attribute()
{
	Attirbute_Levitate();
	Attirbute_New_Levitate();
	Attribute_Guide();
	Attribute_Guide_MeteorOnly();
	Attribute_Ascending();
}

void SkillObject::Attirbute_Levitate()
{
	if (m_skillAttrib & SKILLATTRIBUTE::LEVITATE)
	{
		physx::PxVec3 gravity = PhysDevice::GetInstance()->GetGravity();
		m_body->AddForce(ForceMode::Force, -gravity * m_body->GetMass() * 0.96f);
	}
}

void SkillObject::Attirbute_New_Levitate()
{
	if (m_skillAttrib & SKILLATTRIBUTE::NEW_LEVITATE)
	{
		physx::PxVec3 gravity = PhysDevice::GetInstance()->GetGravity() * 1.00f;
		m_body->AddForce(ForceMode::Force, -gravity * m_body->GetMass());

		physx::PxVec3 currentVel = m_body->GetVelocity();
		if (currentVel.y < 0)
		{
			currentVel.y = 0;
			m_body->SetVelocity(currentVel);
		}
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

void SkillObject::Attribute_Guide_MeteorOnly()
{
	if (!m_target)								//타겟없으면 취소
		return;

	if (m_body->IsExcludedFromSimulation())		//삭제예정이면 취소
		return;

	if (m_skillAttrib & SKILLATTRIBUTE::GUIDED_METEOR)
	{
		float guidePower = 2500.f;

		physx::PxVec3 TargetPos = TO_PX3(m_target->GetTransform()->GetPosition());
		TargetPos.y += 1100.f;
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
		float power = 300.f;

		m_body->SetVelocity(up * power);
	}
}

void SkillObject::Attribute_Descending()
{
	float power = 30.f;

	if (m_body->IsExcludedFromSimulation())		//삭제예정이면 취소
		return;

	if (m_skillAttrib & SKILLATTRIBUTE::ASCENDING)
	{
		physx::PxVec3 down(0, -1, 0);
		down *= power;

		m_body->SetVelocity(down);
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

void SkillObject::SetSkillObjectType(SKILLOBJECTTYPE type)
{
	m_skillType = type;
}

SkillObject::SKILLATTRIBUTE SkillObject::GetAttribute()
{
	return m_skillAttrib;
}

void SkillObject::Nuclear_Attribute_Explosion()
{
	auto layer = ObjectManager::GetInstance()->GetLayer(L"Layer_Player");
	auto players = layer->GetGameObjects();
	for (auto& obj : players)
	{
		Player* player = static_cast<Player*>(obj);
		physx::PxVec3 playerPos = TO_PX3(player->GetControllerPosition());
		playerPos.y = 0;
		physx::PxVec3 skillObjPos = m_body->GetPosition();
		skillObjPos.y = 0;
		physx::PxVec3 knockbackDir = playerPos - skillObjPos;
		float distance = knockbackDir.magnitude();

		if (distance > 1000.f)
			continue;

		knockbackDir.normalize();								//방향 정규화

		auto playerController = player->GetController();
		auto playerBody = playerController->GetBody();

		float verticalStrength = 1.f;
		float horizontalStrength = 1.f;

		if (distance < 300)
		{
			horizontalStrength = 300.f;
			verticalStrength = 1000.f;
		}
		else
		{
			horizontalStrength = 100.f;
			verticalStrength = 600.f;
		}

		playerController->BounceFromAttack();					//플레이어 input 무효화 (땅 착지전까지)
		playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(knockbackDir.x * horizontalStrength, verticalStrength, knockbackDir.z * horizontalStrength));
	}
}
