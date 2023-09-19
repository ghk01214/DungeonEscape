#include "pch.h"
#include "Golem.h"
#include "GolemAI.h"
#include "SkillObject.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "Collider.h"
#include "CollisionPairInfo.h"
#include "CapsuleCollider.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "Player.h"
#include "RigidBody.h"
#include "MonsterAI.h"
#include "EventHandler.h"
#include "OverlapObject.h"
#include "physx_utils.h"

Golem::Golem(int32_t MonsterID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	Monster{ MonsterID, position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState },
	m_sendState{ 0 }
{
	m_name = L"Golem";
	m_objType = server::OBJECT_TYPE::BOSS;
	m_fbxType = server::FBX_TYPE::BLUE_GOLEM;
}

Golem::~Golem()
{
}

void Golem::Init()
{
	Monster::Init();
	m_hp = 1000;
	m_maxHp = 1000;

	m_controller->GetBody()->SetMass(1000.f);
	//SetUp_PatternObjects();

	m_golemAI = new GolemAI(this, 3000.f);
	m_golemAI->Init();

	m_AI = m_golemAI;
	m_overlapObject = new OverlapObject(m_AI);

	m_controller->SetMoveSpeed(20.f);
}

void Golem::Update(double timeDelta)
{
	SendChangedStateAgain();

	m_golemAI->Update(timeDelta);
	if (m_overlapObject)
		m_overlapObject->Update();

	CheckState();				//State를 진입할 때
	UpdateFrameOnce();			//State를 떠날때
	UpdateFrameRepeat();		//State에 있을 때

	Monster::Update(timeDelta);
}

void Golem::LateUpdate(double timeDelta)
{
	Monster::LateUpdate(timeDelta);
}

void Golem::Release()
{
	SafeRelease(m_overlapObject);
	SafeRelease(m_golemAI);


	Monster::Release();
}

void Golem::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case ROAR:
		{

		}
		break;
		case SPELL_START:
		{

		}
		break;
		case SPELL_LOOP:
		{

		}
		break;
		case SPELL_END:
		{

		}
		break;
		case STUN:
		{

		}
		break;
		case ATTACK1:
		{
		}
		break;
		case ATTACK2:
		{
		}
		break;
		case ATTACK3:
		{
		}
		break;
		case ATTACK4:
		{
		}
		break;
		case DAMAGE:
		{
		}
		break;
		case READY:
		{

		}
		break;
		case DEAD:
		{
		}
		break;
		case DEATH:
		{
		}
		break;
		case LAUGH:
		{

		}
		break;
		case TALK:
		{

		}
		break;
		case IDLE1:
		{

		}
		break;
		case IDLE2:
		{

		}
		break;
		case JUMP_START:
		{

		}
		break;
		case JUMP_LOOP:
		{

		}
		break;
		case JUMP_END:
		{

		}
		break;
		case RUN:
		{

		}
		break;
		case RUN_BACKWARD:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
	m_sendState = SEND_AGAIN;

	//for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TimerEvent ev{ ProtocolID::WR_CHANGE_STATE_ACK };
		ev.objID = m_id;
		ev.state = m_currState;
		ev.objType = m_objType;

		MSG_HANDLER->PushSendMessage(ev);
	}
}

void Golem::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case ROAR: case SPELL_START: case SPELL_END: case STUN:
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4:
		case DAMAGE: case READY: case DEATH: case DEAD:
		case JUMP_START: case JUMP_END:
		return;
		default:
		break;
	}
}

void Golem::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case SPELL_LOOP: case LAUGH: case TALK:
		case IDLE1: case IDLE2: case JUMP_LOOP: case SWIM_IDLE:
		case RUN: case RUN_BACKWARD: case RUN_LEFT: case RUN_RIGHT:
		case SWIM: case SWIM_BACKWARD: case SWIM_LEFT: case SWIM_RIGHT:
		case WALK: case WALK_BACKWARD: case WALK_LEFT: case WALK_RIGHT:
		return;
		default:
		break;
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case ROAR: case STUN: case DAMAGE:
		{
			m_currState = IDLE1;
			m_golemAI->UpdateTargetPos();		//패턴이 끝났으면 바로 플레이어를 바라보도록
		}
		break;
		case SPELL_START:
		{
		}
		break;
		case JUMP_START:
		{
			m_currState = IDLE1;
		}
		break;
		case SPELL_END: case JUMP_END:
		{
			m_currState = IDLE1;
		}
		break;
		case ATTACK1: case ATTACK2: case ATTACK3: case ATTACK4:
		{
			m_currState = IDLE1;
		}
		break;
		case READY:
		{
		}
		break;
		case DEATH:
		{
			m_currState = DEAD;
			m_startSendTransform = false;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

GolemAI* Golem::GetAI()
{
	return m_golemAI;
}

void Golem::Pattern_Attack1()
{
	//AddSkillSize에서 줬던 이름으로
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK1");

}

void Golem::Pattern_Attack2()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK2");
}

void Golem::Pattern_Attack3()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK3");
}

void Golem::Pattern_Attack4()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ATTACK4");
}

void Golem::Pattern_Roar()
{
	if (m_overlapObject)
		m_overlapObject->Activate("ROAR");
}

void Golem::Pattern_Run()
{
	if (m_overlapObject)
		m_overlapObject->Activate("RUN");
}

void Golem::Pattern_Spell()
{
	if (m_overlapObject)
		m_overlapObject->Activate("SPELL");
}

void Golem::OverlapObject_Deactivate()
{
	//AddSkillSize에서 줬던 이름으로
	if (m_overlapObject)
		m_overlapObject->Deactivate();
}

void Golem::Pattern_Jump_Ascend()
{
	m_controller->GetBody()->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 1000000.f);
}

void Golem::Pattern_Jump_Select_LandPosition()
{
	std::cout << "landPos selected" << std::endl;
	auto players = ObjectManager::GetInstance()->GetLayer(L"Layer_Player")->GetGameObjects();
	static std::uniform_int_distribution<int> distribution(0, players.size() - 1);
	int randNum = distribution(dre);
	auto it = players.begin();
	std::advance(it, randNum);

	Player* selectedPlayer = dynamic_cast<Player*>(*it);
	m_landingPosition = selectedPlayer->GetControllerPosition();
	m_landingPosition.y += 2500.f;


	RaycastHit buf;
	if (selectedPlayer->GetController()->RaycastGround(buf))
	{
		buf.point;		//위기표시

	}
}

void Golem::Pattern_Jump_Descend()
{
	auto body = m_controller->GetBody();
	body->SetVelocity(physx::PxVec3(0, 0, 0));
	body->SetPosition(m_landingPosition, true);
	body->AddForce(ForceMode::Impulse, physx::PxVec3(0, -1, 0) * 500000.f);
	body->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::SKILLOBJECT_PLAYER,
				static_cast<PhysicsLayers>(PLAYER | SKILLOBJECT_MONSTER));
}

bool Golem::Pattern_Jump_LandCheck()
{
	static std::uniform_real_distribution<float> distribution(0, 2 * physx::PxPi);

	RaycastHit hit;
	m_controller->RaycastGround(hit);
	if (hit.distance < 0 || hit.distance > 5)
		return false;

	auto players = ObjectManager::GetInstance()->GetLayer(L"Layer_Player")->GetGameObjects();
	physx::PxVec3 golemPosXZ = TO_PX3(GetControllerPosition());
	golemPosXZ.y = 0;

	for (auto& obj : players)
	{
		auto player = dynamic_cast<Player*>(obj);
		auto playerController = player->GetController();
		auto playerBody = player->GetController()->GetBody();
		physx::PxVec3 playerPosXZ = playerBody->GetPosition();
		playerPosXZ.y = 0;

		physx::PxVec3 between = playerPosXZ - golemPosXZ;
		float distance = between.magnitude();

		if (playerController->IsOnGround())
		{
			playerBody->SetVelocity(physx::PxVec3(0));
			playerController->BounceFromAttack();		//컨트롤러 넉백 (directionInput봉인)
			if (distance < playerController->GetCollider()->GetRadius() + m_controller->GetCollider()->GetRadius() + 10.f)
			{
				float repelForce = 300.f;
				float theta = distribution(dre);
				physx::PxVec3 randomDir(cos(theta), 0, sin(theta));
				playerBody->AddForce(ForceMode::Impulse, randomDir * repelForce);
				playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 1200.f);

			}
			else										//거리가 멀다면 xzDir로 발사
			{
				physx::PxVec3 xzDir = m_AI->GetXZDir();
				float repelForce = 150.f;
				playerBody->AddForce(ForceMode::Impulse, xzDir * repelForce);
				playerBody->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1, 0) * 800.f);
			}
		}
	}

	EventHandler::GetInstance()->AddEvent("GOLEM_PHYSICLAYER_TO_DEFAULT", 0.5f, this);		//continous

	ServerMessage_SendRenderLandEffect(GetControllerPosition());

	game::TimerEvent ev{ ProtocolID::WR_CHANGE_SOUND_ACK };
	ev.objID = m_id;
	ev.state = magic_enum::enum_integer(server::SOUND_TYPE::LANDING);

	MSG_HANDLER->PushSendMessage(ev);

	return true;
}

bool Golem::PhysicLayer_SetToDefault()
{
	auto players = ObjectManager::GetInstance()->GetLayer(L"Layer_Player")->GetGameObjects();
	for (auto& obj : players)
	{
		auto player = dynamic_cast<Player*>(obj);
		auto playerBody = player->GetController()->GetBody();
		physx::PxVec3 playerPos = playerBody->GetPosition();
		physx::PxVec3 golemPos = TO_PX3(GetControllerPosition());
		physx::PxVec3 between = playerPos - golemPos;
		float distance = between.magnitude();
		if (distance < 300.f)										//최소 골렘과 150 떨어져야만 몬스터의 물리 레이어를 원래대로 되돌린다.
			return false;
	}

	m_controller->GetBody()->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::MONSTER, PhysicsLayers::SKILLOBJECT_MONSTER);
	return true;
}

Golem::GOLEM_STATE Golem::GetState() const
{
	return m_currState;
}

void Golem::SetState(GOLEM_STATE state)
{
	m_currState = state;
}

void Golem::ServerMessage_SendRenderLandEffect(const Vec3& pos)
{
	game::TimerEvent ev{ ProtocolID::WR_RENDER_EFFECT_ACK };
	ev.objID = m_id;
	ev.state = magic_enum::enum_integer(server::EFFECT_TYPE::IN_DISPERSAL);
	ev.effectPos = pos;

	MSG_HANDLER->PushSendMessage(ev);
}

void Golem::SendChangedStateAgain()
{
	if (m_sendState <= 0)
		return;

	game::TimerEvent ev{ ProtocolID::WR_CHANGE_STATE_ACK };
	ev.objID = m_id;
	ev.state = m_currState;
	ev.objType = m_objType;

	MSG_HANDLER->PushSendMessage(ev);

	--m_sendState;
}
